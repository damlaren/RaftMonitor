/* Copyright (c) 2012 Stanford University
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

#include <cinttypes>
#include <google/protobuf/message.h>
#include <iostream>
#include <memory>
#include <string>

#include "RPC/Buffer.h"
#include "RPC/OpaqueClientRPC.h"

#ifndef LOGCABIN_RPC_CLIENTRPC_H
#define LOGCABIN_RPC_CLIENTRPC_H

namespace LogCabin {
namespace RPC {

class ClientSession; // forward declaration

/**
 * This class represents an asynchronous remote procedure call. Unlike
 * OpaqueClientRPC, this executes a particular method on a particular service.
 *
 * TODO(ongaro): This abstraction should ideally contain protocol version
 * negotiation for the higher layer. To do so, we'd want to be able to
 * associate a small amount of state with the session.
 */
class ClientRPC {
  public:

    /**
     * Issue an RPC to a remote service.
     * \param session
     *      A connection to the remote server.
     * \param service
     *      Identifies the service running on the server.
     *      See Protocol::Common::ServiceId.
     * \param serviceSpecificErrorVersion
     *      This tells the Service what service-specific errors the client
     *      understands. Clients can expect services to not send
     *      service-specific errors introduced in newer versions, but they
     *      should remain compatible with older versions.
     * \param opCode
     *      Identifies the remote procedure within the Service to execute.
     * \param request
     *      The arguments to the remote procedure.
     */
    ClientRPC(std::shared_ptr<RPC::ClientSession> session,
              uint16_t service,
              uint8_t serviceSpecificErrorVersion,
              uint16_t opCode,
              const google::protobuf::Message& request);

    /**
     * Default constructor. This doesn't create a valid RPC, but it is useful
     * as a placeholder.
     */
    ClientRPC();

    /**
     * Move constructor.
     */
    ClientRPC(ClientRPC&&);

    /**
     * Destructor.
     */
    ~ClientRPC();

    /**
     * Move assignment.
     */
    ClientRPC& operator=(ClientRPC&&);

    /**
     * Abort the RPC.
     * The caller is no longer interested in its reply.
     */
    void cancel();

    /**
     * Indicate whether a response or error has been received for
     * the RPC.
     * \return
     *      True means the reply is ready or an error has occurred; false
     *      otherwise.
     */
    bool isReady();

    /**
     * The return type of waitForReply().
     */
    enum class Status {
        /**
         * The service returned a normal response. This is available in
         * 'response'.
         */
        OK,
        /**
         * The service threw an error (but at the transport level, the RPC
         * succeeded). Service-specific details may be available in
         * 'serviceSpecificError'.
         */
        SERVICE_SPECIFIC_ERROR,
        /**
         * The server could not be contacted or did not reply. It is unknown
         * whether or not the server executed the RPC. More information is
         * available with getErrorMessage().
         */
        RPC_FAILED,
    };

    /**
     * Wait for a reply to the RPC or an error.
     * Panics if the server responds but is not running the same protocol.
     *
     * \param[out] response
     *      If not NULL, this will be filled in if this method returns OK.
     * \param[out] serviceSpecificError
     *      If not NULL, this will be filled in if this method returns
     *      SERVICE_SPECIFIC_ERROR.
     * \return
     *      See the individual values of #Status.
     */
    Status waitForReply(google::protobuf::Message* response,
                        google::protobuf::Message* serviceSpecificError);

    /**
     * If an RPC failure occurred, return a message describing that error.
     *
     * All errors indicate that it is unknown whether or not the server
     * executed the RPC. Unless the RPC was canceled with #cancel(), the
     * ClientSession has been disconnected and is no longer useful for
     * initiating new RPCs.
     *
     * \return
     *      If an RPC failure has occurred, a message describing that error.
     *      Otherwise, an empty string.
     */
    std::string getErrorMessage() const;

  private:

    OpaqueClientRPC opaqueRPC;

    // ClientRPC is non-copyable.
    ClientRPC(const ClientRPC&) = delete;
    ClientRPC& operator=(const ClientRPC&) = delete;

}; // class ClientRPC

/**
 * Output a ClientRPC::Status to a stream.
 * This is helpful for google test output.
 */
::std::ostream&
operator<<(::std::ostream& os, ClientRPC::Status status);

} // namespace LogCabin::RPC
} // namespace LogCabin

#endif /* LOGCABIN_RPC_CLIENTRPC_H */
