#pragma once
#include <Windows.h>

#include <chrono>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Config.h"
#include "ThreadedQueue.h"
#include "Message.h"

namespace producer {
void reader_thread(const std::string& comport,
                  const std::chrono::seconds duration, ThreadedQueue& queue);
}
