#pragma once
#include <chrono>
#include <string>

#include "ThreadedQueue.h"

namespace producer {
void reader_thread(const std::string& comport,
                  const std::chrono::seconds duration, ThreadedQueue& queue);
}
