#include "pch.h"
#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <string>
#include <sstream>

#include "Message.h"

namespace {
std::string to_output_string(const Message& message) { 
	std::ostringstream oss;
    oss << message;
    return oss.str();
}
}

class MessageTests : public::testing::Test {
   protected:
    Message valid_message_ { 
        .bytes { 0x55, 0x01, 0x00, 0xFF, 0xFF },
        .checksum = 171, 
    };
    Message valid_zero_ { 
        .bytes = { 0x55, 0x00, 0x00, 0x00, 0x00 },
        .checksum = 170,
    };
    // Sum of bytes is 0x0451 (1105 > 255).
    // Wrapped to u8 -> 0x51 (81); bitwise NOT !0x51 = 0xAE (174).
    Message valid_wrapped_ {
        .bytes = { 0x55, 0xFF, 0xFF, 0xFF, 0xFF },
        .checksum = 174,
    };
};

TEST_F(MessageTests, CombineMsbLsbHandlesPositiveNegative) {
    EXPECT_EQ(Message::combine_msb_lsb(0x01U, 0x00U), 256);
    EXPECT_EQ(Message::combine_msb_lsb(0xFFU, 0xFFU), -1);
}

TEST_F(MessageTests, CombineMsbLsbHandlesExtremes) {
    EXPECT_EQ(Message::combine_msb_lsb(0x80U, 0x00U), std::numeric_limits<int16_t>::min());
    EXPECT_EQ(Message::combine_msb_lsb(0x7FU, 0xFFU), std::numeric_limits<int16_t>::max());
}

TEST_F(MessageTests, VerifiedValidChecksum) { 
    EXPECT_TRUE(valid_message_.verify());
}

TEST_F(MessageTests, RejectsBadChecksum) {
    Message bad_checksum = valid_message_;
    bad_checksum.checksum += 1;

    EXPECT_FALSE(bad_checksum.verify());
}

TEST_F(MessageTests, RejectsCorruptedPayload) {
    Message corrupted_message = valid_message_;

    // Flip a bit in a bytes[2]
    corrupted_message.bytes[2] ^= 0x01;

    EXPECT_FALSE(corrupted_message.verify());
}

TEST_F(MessageTests, VerifiesChecksumWhenByteSumExceeds255) {
    // Sum of bytes is 0x0451 (1105 > 255).
    // Wrapped to u8 -> 0x51 (81); bitwise NOT !0x51 = 0xAE (174).
    EXPECT_EQ(valid_wrapped_.bytes[0], 0x55);
    EXPECT_EQ(valid_wrapped_.bytes[1], 0xFF);
    EXPECT_EQ(valid_wrapped_.bytes[2], 0xFF);
    EXPECT_EQ(valid_wrapped_.bytes[3], 0xFF);
    EXPECT_EQ(valid_wrapped_.bytes[4], 0xFF);
    EXPECT_EQ(valid_wrapped_.checksum, 174);
    EXPECT_TRUE(valid_wrapped_.verify());
}

TEST_F(MessageTests, StreamsMessageInExpectedCsvFormat) {
    EXPECT_EQ(to_output_string(valid_message_), "85,1,0,256,255,255,-1,171\n");
}

TEST_F(MessageTests, HandlesZeroedData) {
    EXPECT_TRUE(valid_zero_.verify());
    EXPECT_EQ(to_output_string(valid_zero_), "85,0,0,0,0,0,0,170\n");
}
