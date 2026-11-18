#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace config {
// Serial Port Communication Settings
constexpr uint32_t BAUD_RATE = 1036800;
constexpr uint8_t BYTE_SIZE = 8;
constexpr size_t SERIAL_BUFFER_SIZE = 128;

// Packet Framing Constants
constexpr uint8_t SYNC_BYTE = 0x55;
constexpr size_t PACKET_SIZE = 6;  // Sync byte (1) + Data bytes (4) + Checksum (1)

// Subsampling display rate (~3 Hz)
constexpr auto PRINT_SUBSAMPLE_RATE = std::chrono::milliseconds(1000 / 3);

// Default configuration values
inline const std::string DEFAULT_OUTPUT_FILENAME = "output.csv";
}