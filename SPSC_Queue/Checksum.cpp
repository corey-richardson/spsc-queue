#include "Checksum.h"

// Sum message.bytes
// Ones complement
bool checksum::verify(const Message &message) {
    uint8_t sum = 0;
    for (uint8_t byte : message.bytes) {
        sum += byte;
    }
    sum = ~sum;

    return message.checksum == sum;
}
