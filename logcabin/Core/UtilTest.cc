/* Copyright (c) 2011-2012 Stanford University
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

#include "Core/Util.h"

namespace LogCabin {
namespace Core {
namespace Util {
namespace {

TEST(CoreUtilTest, downCast) {
#if !NDEBUG
    EXPECT_DEATH(downCast<uint8_t>(256), "");
    EXPECT_DEATH(downCast<int8_t>(192), "");
    EXPECT_DEATH(downCast<uint8_t>(-10), "");
#endif
    uint8_t x = downCast<uint8_t>(55UL);
    EXPECT_EQ(55, x);
}

TEST(CoreUtilTest, sizeof32) {
    EXPECT_EQ(8U, sizeof32(uint64_t));
}

} // namespace LogCabin::Core::Util::<anonymous>
} // namespace LogCabin::Core::Util
} // namespace LogCabin::Core
} // namespace LogCabin
