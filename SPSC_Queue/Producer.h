#pragma once
#include <Windows.h>

#include <chrono>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Checksum.h"
#include "Config.h"
#include "ThreadedQueue.h"
#include "Types.h"

namespace producer {
void readerThread(const std::string& comport,
                  const std::chrono::seconds duration, ThreadedQueue& queue);
}
