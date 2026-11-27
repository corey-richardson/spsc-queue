#pragma once

#include <chrono>
#include <string>

struct CliArgs {
	std::string raw_comport;
    std::string comport;
    std::chrono::seconds duration { 0 };
    std::string output_filename;
};

std::string get_timestamped_filename();
CliArgs parse_cli_args(int argc, char* argv[]);
