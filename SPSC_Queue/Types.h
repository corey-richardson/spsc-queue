#pragma once
#include <array>
#include <iostream>

// Holds 1 sync byte, 4 data bytes, and checksum byte
struct Message {
    std::array<uint8_t, 5> bytes;
    uint8_t checksum;

    // Combines two unsigned 8-bit bytes into a single
    // signed 16-bit integer
    // MSB: shift 8 bits left; 0xFF -> 0xFF00
    // LSB: concatenate with MSB; 0xFF00 + 0x11 -> 0xFF11
    static int16_t combineMsbLsb(uint8_t msb, uint8_t lsb) {
        return static_cast<int16_t>((msb << 8) | lsb);
    }
};

// Overloaded << operator, allowing control of the stream
// that a message is outputted to; console or file stream.
inline std::ostream& operator<<(std::ostream& stream, const Message& message) {
    return (stream << static_cast<int>(message.bytes[0]) << ","  // Sync Byte
                   << static_cast<int>(message.bytes[1]) << ","
                   << static_cast<int>(message.bytes[2]) << ","
                   << message.combineMsbLsb(message.bytes[1], message.bytes[2])
                   << "," << static_cast<int>(message.bytes[3]) << ","
                   << static_cast<int>(message.bytes[4]) << ","
                   << message.combineMsbLsb(message.bytes[3], message.bytes[4])
                   << "," << static_cast<int>(message.checksum) << "\n");
}
