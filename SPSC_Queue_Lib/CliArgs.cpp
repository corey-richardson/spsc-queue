#include "CliArgs.h"

#include <chrono>
#include <stdexcept>
#include <string>

std::string get_timestamped_filename() {
    auto now = std::chrono::floor<std::chrono::seconds>(
        std::chrono::current_zone()->to_local(
            std::chrono::system_clock::now()));

    return std::format("{:%Y-%m-%d_%H-%M-%S}.csv", now);
}

CliArgs parse_cli_args(int argc, char* argv[]) {
    if (argc < 3) {
        throw std::invalid_argument("Missing required arguments.");
    }

    const std::string raw_comport = argv[1];
    const std::string comport = "\\\\.\\" + raw_comport;

    const std::chrono::seconds duration(std::stoi(argv[2]));

    const std::string output_filename =
        "Output/" + ((argc > 3) ? (std::string(argv[3]) + ".csv")
                                : get_timestamped_filename());

    return CliArgs{
        .raw_comport = raw_comport,
        .comport = comport,
        .duration = duration,
        .output_filename = output_filename,
    };
}
