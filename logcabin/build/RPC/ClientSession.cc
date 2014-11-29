/* Copyright (c) 2012-2014 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <string.h>
#include <unistd.h>

#include "Core/Debug.h"
#include "Core/StringUtil.h"
#include "RPC/ClientSession.h"

/**
 * The number of milliseconds to wait until the client gets suspicious about
 * the server not responding. After this amount of time elapses, the client
 * will send a ping to the server. If no response is received within another
 * TIMEOUT_MS milliseconds, the session is closed.
 *
 * TODO(ongaro): How should this value be chosen?
 * Ideally, you probably want this to be set to something like the 99-th
 * percentile of your RPC latency.
 *
 * TODO(ongaro): How does this interact with TCP?
 */
enum { TIMEOUT_MS = 100 };

/**
 * A message ID reserved for ping messages used to check the server's liveness.
 * No real RPC will ever be assigned this ID.
 */
enum { PING_MESSAGE_ID = 0 };

namespace LogCabin {
namespace RPC {

////////// ClientSession::ClientMessageSocket //////////

ClientSession::ClientMessageSocket::ClientMessageSocket(
        ClientSession& session,
        int fd,
        uint32_t maxMessageLength)
    : MessageSocket(session.eventLoop, fd, maxMessageLength)
    , session(session)
{
}

void
ClientSession::ClientMessageSocket::onReceiveMessage(MessageId messageId,
                                                     Buffer message)
{
    std::unique_lock<std::mutex> mutexGuard(session.mutex);

    if (messageId == PING_MESSAGE_ID) {
        if (session.numActiveRPCs > 0 && session.activePing) {
            // The server has shown that it is alive for now.
            // Let's get suspicious again in another TIMEOUT_MS.
            session.activePing = false;
            session.timer.schedule(TIMEOUT_MS * 1000 * 1000);
        } else {
            VERBOSE("Received an unexpected ping response. This can happen "
                    "for a number of reasons and is no cause for alarm. For "
                    "example, this happens if a ping request was sent out, "
                    "then all RPCs completed before the ping response "
                    "arrived.");
        }
        return;
    }

    auto it = session.responses.find(messageId);
    if (it == session.responses.end()) {
        VERBOSE("Received an unexpected response with message ID %lu. "
                "This can happen for a number of reasons and is no cause "
                "for alarm. For example, this happens if the RPC was "
                "cancelled before its response arrived.",
                messageId);
        return;
    }
    Response& response = *it->second;
    if (response.status == Response::HAS_REPLY) {
        WARNING("Received a second response from the server for "
                "message ID %lu. This indicates that either the client or "
                "server is assigning message IDs incorrectly, or "
                "the server is misbehaving. Dropped this response.",
                messageId);
        return;
    }

    // Book-keeping for timeouts
    --session.numActiveRPCs;
    if (session.numActiveRPCs == 0)
        session.timer.deschedule();
    else
        session.timer.schedule(TIMEOUT_MS * 1000 * 1000);

    // Fill in the response
    response.status = Response::HAS_REPLY;
    response.reply = std::move(message);
    response.ready.notify_all();
}

void
ClientSession::ClientMessageSocket::onDisconnect()
{
    VERBOSE("Disconnected from server %s",
            session.address.toString().c_str());
    std::unique_lock<std::mutex> mutexGuard(session.mutex);
    if (session.errorMessage.empty()) {
        // Fail all current and future RPCs.
        session.errorMessage = ("Disconnected from server " +
                                session.address.toString());
        // Notify any waiting RPCs.
        for (auto it = session.responses.begin();
             it != session.responses.end();
             ++it) {
            Response* response = it->second;
            response->ready.notify_all();
        }
    }
}

////////// ClientSession::Response //////////

ClientSession::Response::Response()
    : status(Response::WAITING)
    , reply()
    , hasWaiter(false)
    , ready()
{
}

////////// ClientSession::Timer //////////

ClientSession::Timer::Timer(ClientSession& session)
    : Event::Timer(session.eventLoop)
    , session(session)
{
}

void
ClientSession::Timer::handleTimerEvent()
{
    std::unique_lock<std::mutex> mutexGuard(session.mutex);

    // Handle "spurious" wake-ups.
    if (!session.messageSocket ||
        session.numActiveRPCs == 0 ||
        !session.errorMessage.empty()) {
        return;
    }

    // Send a ping or expire the session.
    if (!session.activePing) {
        VERBOSE("ClientSession is suspicious. Sending ping.");
        session.activePing = true;
        session.messageSocket->sendMessage(PING_MESSAGE_ID, Buffer());
        schedule(TIMEOUT_MS * 1000 * 1000);
    } else {
        VERBOSE("ClientSession to %s timed out.",
                session.address.toString().c_str());
        // Fail all current and future RPCs.
        session.errorMessage = ("Server " +
                                session.address.toString() +
                                " timed out");
        // Notify any waiting RPCs.
        for (auto it = session.responses.begin();
             it != session.responses.end();
             ++it) {
            Response* response = it->second;
            response->ready.notify_all();
        }
    }
}

////////// ClientSession //////////

ClientSession::ClientSession(Event::Loop& eventLoop,
                             const Address& address,
                             uint32_t maxMessageLength)
    : self() // makeSession will fill this in shortly
    , eventLoop(eventLoop)
    , address(address)
    , messageSocket()
    , timer(*this)
    , mutex()
    , nextMessageId(1) // 0 is reserved for PING_MESSAGE_ID
    , responses()
    , errorMessage()
    , numActiveRPCs(0)
    , activePing(false)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        errorMessage = "Failed to create socket";
        return;
    }
    int r = connect(fd,
                    address.getSockAddr(),
                    address.getSockAddrLen());
    if (r != 0) {
        errorMessage = "Failed to connect socket to " + address.toString();
        close(fd);
        return;
    }
    messageSocket.reset(new ClientMessageSocket(*this, fd, maxMessageLength));
}

std::shared_ptr<ClientSession>
ClientSession::makeSession(Event::Loop& eventLoop,
                           const Address& address,
                           uint32_t maxMessageLength)
{
    std::shared_ptr<ClientSession> session(
        new ClientSession(eventLoop, address, maxMessageLength));
    session->self = session;
    return session;
}


ClientSession::~ClientSession()
{
    timer.deschedule();
    for (auto it = responses.begin(); it != responses.end(); ++it)
        delete it->second;
}

OpaqueClientRPC
ClientSession::sendRequest(Buffer request)
{
    MessageSocket::MessageId messageId;
    {
        std::unique_lock<std::mutex> mutexGuard(mutex);
        messageId = nextMessageId;
        ++nextMessageId;
        responses[messageId] = new Response();

        ++numActiveRPCs;
        if (numActiveRPCs == 1) {
            // activePing's value was undefined while numActiveRPCs = 0
            activePing = false;
            timer.schedule(TIMEOUT_MS * 1000 * 1000);
        }
    }
    // Release the mutex before sending so that receives can be processed
    // simultaneously with sends.
    if (messageSocket)
        messageSocket->sendMessage(messageId, std::move(request));
    OpaqueClientRPC rpc;
    rpc.session = self.lock();
    rpc.responseToken = messageId;
    return rpc;
}

std::string
ClientSession::getErrorMessage() const
{
    std::unique_lock<std::mutex> mutexGuard(mutex);
    return errorMessage;
}

std::string
ClientSession::toString() const
{
    std::string error = getErrorMessage();
    if (error.empty()) {
        return "Active session to " + address.toString();
    } else {
        // error will already include the server's address.
        return "Closed session: " + error;
    }
}

void
ClientSession::cancel(OpaqueClientRPC& rpc)
{
    // The RPC may be holding the last reference to this session. This
    // temporary reference makes sure this object isn't destroyed until after
    // we return from this method. It must be the first line in this method.
    std::shared_ptr<ClientSession> selfGuard(self.lock());

    // There are two ways to cancel an RPC:
    // 1. If there's some thread currently blocked in wait(), this method marks
    //    the Response's status as CANCELED, and wait() will delete it later.
    // 2. If there's no thread currently blocked in wait(), the Response is
    //    deleted entirely.
    std::unique_lock<std::mutex> mutexGuard(mutex);
    auto it = responses.find(rpc.responseToken);
    if (it == responses.end())
        return;
    Response* response = it->second;
    if (response->hasWaiter) {
        response->status = Response::CANCELED;
        response->ready.notify_all();
    } else {
        delete response;
        responses.erase(it);
    }

    --numActiveRPCs;
    // Even if numActiveRPCs == 0, it's simpler here to just let the timer wake
    // up an extra time and clean up. Otherwise, we'd need to grab an
    // Event::Loop::Lock prior to the mutex to call deschedule() without
    // inducing deadlock.
}

void
ClientSession::update(OpaqueClientRPC& rpc)
{
    // The RPC may be holding the last reference to this session. This
    // temporary reference makes sure this object isn't destroyed until after
    // we return from this method. It must be the first line in this method.
    std::shared_ptr<ClientSession> selfGuard(self.lock());

    std::unique_lock<std::mutex> mutexGuard(mutex);
    auto it = responses.find(rpc.responseToken);
    if (it == responses.end()) {
        // RPC was cancelled, fields set already
        assert(rpc.ready);
        return;
    }
    Response* response = it->second;
    if (response->status == Response::HAS_REPLY) {
        rpc.reply = std::move(response->reply);
    } else if (!errorMessage.empty()) {
        rpc.errorMessage = errorMessage;
    } else {
        // If the RPC was canceled, then it'd be marked ready and update()
        // wouldn't be called again.
        assert(response->status != Response::CANCELED);
        return; // not ready
    }
    rpc.ready = true;
    rpc.session.reset();

    delete response;
    responses.erase(it);
}

void
ClientSession::wait(const OpaqueClientRPC& rpc)
{
    // The RPC may be holding the last reference to this session. This
    // temporary reference makes sure this object isn't destroyed until after
    // we return from this method. It must be the first line in this method.
    std::shared_ptr<ClientSession> selfGuard(self.lock());

    std::unique_lock<std::mutex> mutexGuard(mutex);
    while (true) {
        auto it = responses.find(rpc.responseToken);
        if (it == responses.end())
            return; // RPC was cancelled or already updated
        Response* response = it->second;
        if (response->status == Response::HAS_REPLY) {
            return; // RPC has completed
        } else if (response->status == Response::CANCELED) {
            // RPC was cancelled, finish cleaning up
            delete response;
            responses.erase(it);
            return;
        } else if (!errorMessage.empty()) {
            return; // session has error
        }
        response->hasWaiter = true;
        response->ready.wait(mutexGuard);
        response->hasWaiter = false;
    }
}

} // namespace LogCabin::RPC
} // namespace LogCabin
