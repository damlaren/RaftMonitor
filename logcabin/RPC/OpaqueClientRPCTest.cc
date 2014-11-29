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

#include <gtest/gtest.h>

#include "Core/Debug.h"
#include "RPC/OpaqueClientRPC.h"

namespace LogCabin {
namespace RPC {
namespace {

TEST(RPCOpaqueClientRPCTest, constructor_default) {
    // make sure a default-constructed RPC object behaves sensibly
    OpaqueClientRPC rpc;
    EXPECT_EQ("", rpc.getErrorMessage());
    EXPECT_FALSE(rpc.isReady());
    EXPECT_THROW(rpc.extractReply(),
                 OpaqueClientRPC::Error);
    rpc.waitForReply();
    EXPECT_TRUE(rpc.isReady());
    EXPECT_EQ("This RPC was never associated with a ClientSession.",
              rpc.getErrorMessage());

    rpc = OpaqueClientRPC();
    rpc.cancel();
    EXPECT_TRUE(rpc.isReady());
    EXPECT_EQ("RPC canceled by user",
              rpc.getErrorMessage());
}

TEST(RPCOpaqueClientRPCTest, constructor_move) {
    // nothing to test
}

TEST(RPCOpaqueClientRPCTest, destructor) {
    // nothing to test
}

TEST(RPCOpaqueClientRPCTest, assignment_move) {
    // nothing to test
}

TEST(RPCOpaqueClientRPCTest, cancel) {
    // tested in RPCClientSessionTest
}

TEST(RPCOpaqueClientRPCTest, extractReply) {
    OpaqueClientRPC rpc;
    rpc.ready = true;
    rpc.errorMessage = "error";
    EXPECT_THROW(rpc.extractReply(),
                 OpaqueClientRPC::Error);
    rpc.errorMessage = "";
    rpc.reply = Buffer(NULL, 3, NULL);
    EXPECT_EQ(3U, rpc.extractReply().getLength());
    EXPECT_EQ(0U, rpc.extractReply().getLength());
}

TEST(RPCOpaqueClientRPCTest, getErrorMessage) {
    OpaqueClientRPC rpc;
    rpc.ready = true;
    rpc.errorMessage = "error";
    EXPECT_EQ("error", rpc.getErrorMessage());
}

TEST(RPCOpaqueClientRPCTest, isReady) {
    OpaqueClientRPC rpc;
    rpc.ready = true;
    EXPECT_TRUE(rpc.isReady());
}

TEST(RPCOpaqueClientRPCTest, peekReply) {
    OpaqueClientRPC rpc;
    rpc.ready = true;
    rpc.reply = Buffer(NULL, 3, NULL);
    EXPECT_EQ(3U, rpc.peekReply()->getLength());
    rpc.errorMessage = "foo";
    EXPECT_TRUE(rpc.peekReply() == NULL);
}

TEST(RPCOpaqueClientRPCTest, waitForReply) {
    // tested in RPCClientSessionTest
}

TEST(RPCOpaqueClientRPCTest, update) {
    // tested in RPCClientSessionTest
}

} // namespace LogCabin::RPC::<anonymous>
} // namespace LogCabin::RPC
} // namespace LogCabin
