#include <iostream>
#include <stdexcept>
#include <gtest/gtest.h>
#include "utils.h"
#include "endianness.h"
#include "ip_address.h"
#include "ipv6_address.h"

using namespace Tins;

class UtilsTest : public testing::Test {
public:
    static const uint32_t zero_int_ip; // "0.0.0.0"
    static const uint32_t full_int_ip; // "255.255.255.255"
    static const uint32_t mix_int_ip; // "1.2.255.3"
    static const uint8_t zero_hw_addr[];
    static const uint8_t full_hw_addr[];
    static const uint8_t mix_hw_addr[];
    static const uint8_t data[];
    static const uint32_t data_len;

};

const uint32_t UtilsTest::zero_int_ip = 0; // "0.0.0.0"
const uint32_t UtilsTest::full_int_ip = 0xFFFFFFFF; // "255.255.255.255"
const uint32_t UtilsTest::mix_int_ip = 0x0102FF03; // "1.2.255.3"
const uint8_t UtilsTest::zero_hw_addr[] = {
    0, 0, 0, 0, 0, 0
};
const uint8_t UtilsTest::full_hw_addr[] = {
    255, 255, 255, 255, 255, 255
};
const uint8_t UtilsTest::mix_hw_addr[] = {
    1, 2, 3, 4, 5, 6
};
const uint8_t UtilsTest::data[] = {
    215, 3, 132, 147, 170, 153, 240, 223, 227, 47, 144, 146, 52, 154, 192, 106,
    195, 167, 160, 119, 154, 134, 59, 150, 6, 236, 67, 216, 7, 19, 110, 226, 228,
    99, 103, 18, 39, 23, 157, 192, 38, 37, 23, 211, 77, 86, 176, 103, 62, 226, 235,
    236, 114, 39, 216, 202, 236, 251, 158, 249, 89, 248, 80, 95, 245, 35, 61, 97,
    242, 90, 122, 196, 187, 202, 15, 197, 204, 180, 183, 68, 65, 99, 209, 172, 80,
    189, 188, 18, 216, 82, 103, 53, 3, 246, 183, 4, 56, 201, 5, 42, 94, 108, 30, 90,
    183, 203, 193, 19, 128, 79, 156, 189, 18, 163, 67, 152, 153, 114, 151, 125, 114,
    87, 105, 31, 212, 238, 154, 238, 82, 216, 244, 2, 33, 137, 126, 67, 176, 224, 95,
    175, 205, 175, 91, 41, 101, 34, 178, 199, 88, 211, 91, 235, 42, 21, 182, 138,
    185, 61, 205, 61, 245, 85, 18, 119, 253, 214, 127, 164, 31, 225, 140, 58, 103,
    235, 231, 226, 119, 97, 86, 11, 56, 95, 218, 207, 137, 216, 141, 46, 82, 39, 158,
    243, 131, 217, 197, 177, 239, 30, 145, 249, 162, 141, 252, 213, 132, 87, 42, 130,
    213, 92, 47, 163, 113, 230, 59, 205, 19, 90, 65, 134, 181, 44, 150, 254, 73, 186,
    194, 122, 96, 65, 114, 233, 245, 25, 194, 80, 174, 223, 158, 45, 131, 188, 222,
    52, 212, 250, 96, 172, 181, 115, 252, 40, 249, 99, 65, 23, 118, 71, 124, 112, 228,
    204, 106, 169, 40, 148, 72, 183, 252, 234, 83, 116, 109, 54, 233, 58, 231, 5, 88,
    36, 77, 253, 75, 90, 250, 177, 159, 199, 180, 134, 211, 161, 175, 75, 161, 72, 80,
    73, 163, 76, 160, 119, 226, 248, 231, 62, 91, 74, 32, 156, 9, 64, 170, 79, 38, 45,
    204, 58, 144, 76, 226, 130, 21, 151, 239, 40, 116, 52, 77, 18, 6, 199, 42, 200, 213,
    232, 12, 61, 156, 51, 23, 165, 11, 7, 149, 30, 27, 119, 216, 246, 93, 24, 111, 105,
    218, 100, 45, 57, 69, 229, 168, 105, 99, 35, 41, 71, 255, 80, 255, 22, 7, 61, 211,
    134, 113, 48, 255, 220, 26, 32, 6, 184, 204, 40, 194, 47, 201, 249, 133, 194, 203,
    172, 123, 186, 77, 39, 92, 64, 52, 91, 187, 83, 58, 73, 65, 192, 150, 103, 230, 187,
    165, 149, 84, 71, 142, 55, 69, 87, 102, 97, 20, 134, 184, 107, 133, 57, 57, 220, 121,
    211, 241, 97, 172, 67, 208, 9, 151, 14, 200, 73, 31, 140, 34, 176, 215, 111, 4, 143,
    13, 173, 193, 145, 255, 112, 249, 191, 88, 181, 113, 221, 50, 45, 34, 176, 203, 154,
    65, 193, 6, 120, 182, 235, 250, 21, 136, 44, 21, 29, 6, 150, 194, 117, 118, 237, 0,
    223, 207, 161, 58, 229, 174, 101, 101, 195, 17, 249, 12, 137, 177, 161
    };
const uint32_t UtilsTest::data_len = 500;


TEST_F(UtilsTest, Crc32) {

    uint32_t crc = Utils::crc32(data, data_len);

    EXPECT_EQ(crc, 0x78840f54U);

}

TEST_F(UtilsTest, ResolveDomain) {
    IPv4Address localhost_ip("127.0.0.1");

    EXPECT_EQ(Utils::resolve_domain("localhost"), localhost_ip);
}

TEST_F(UtilsTest, ResolveDomain6) {
    IPv6Address localhost_ip("2606:2800:220:6d:26bf:1447:1097:aa7");

    EXPECT_EQ(Utils::resolve_domain6("example.com"), localhost_ip);
}

// FIXME
TEST_F(UtilsTest, Checksum) {

    /*uint16_t checksum = Utils::do_checksum(data, data + data_len);

    //EXPECT_EQ(checksum, 0x231a);

    uint8_t my_data[] = {0, 0, 0, 0};
    checksum = Utils::do_checksum(my_data, my_data + 4);
    //EXPECT_EQ(checksum, 0xFFFF);
*/
}
