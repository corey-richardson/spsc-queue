#pragma once
#include <cstdint>

#include "Message.h"

namespace checksum {
bool verify(const Message& message);
}
