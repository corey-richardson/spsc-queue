#include "pch.h"
#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <stdexcept>

#include "CliArgs.h"

TEST(CliArgsTests, TimestampedFilenameFormat) {
    const std::string name = get_timestamped_filename();

    ASSERT_FALSE(name.empty());
    EXPECT_EQ(name.size(), 23);
    EXPECT_TRUE(name.ends_with(".csv"));
}

TEST(CliArgsTests, ThrowWhenMissingArgs) { 
	std::array<char *, 2> argv { 
        const_cast<char *>("SPSC_Queue.exe"),
        const_cast<char *>("COM4"),
    };

    EXPECT_THROW(parse_cli_args(2, argv.data()), std::invalid_argument);
}

TEST(CliArgsTests, ThrowsWhenDurationInvalid) {
    std::array<char *, 3> argv { 
        const_cast<char *>("SPSC_Queue.exe"),
        const_cast<char *>("COM4"),
        const_cast<char *>("abc"),
    };

    EXPECT_THROW(parse_cli_args(3, argv.data()), std::invalid_argument);
}

TEST(CliArgsTests, ParsesArgsCreatesTimestampedFile) {
    std::array<char *, 3> argv {
        const_cast<char *>("SPSC_Queue.exe"),
        const_cast<char *>("COM4"),
        const_cast<char *>("120"),
    };

    CliArgs parsed {};

    ASSERT_NO_THROW(parsed = parse_cli_args(3, argv.data()));

    EXPECT_EQ(parsed.raw_comport, "COM4");
    EXPECT_EQ(parsed.comport, "\\\\.\\COM4");
    EXPECT_EQ(parsed.duration, std::chrono::seconds { 120 });
    EXPECT_TRUE(parsed.output_filename.starts_with("Output/"));
    EXPECT_TRUE(parsed.output_filename.ends_with(".csv"));
}

TEST(CliArgsTests, ParsesArgsCustomFilename) {
    std::array<char *, 4> argv {
        const_cast<char *>("SPSC_Queue.exe"),
        const_cast<char *>("COM4"),
        const_cast<char *>("120"),
        const_cast<char *>("test_file"),
    };

    CliArgs parsed {};

    ASSERT_NO_THROW(parsed = parse_cli_args(4, argv.data()));

    EXPECT_EQ(parsed.output_filename, "Output/test_file.csv");
}
