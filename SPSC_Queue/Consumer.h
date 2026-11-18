#pragma once
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Config.h"
#include "ThreadedQueue.h"
#include "Message.h"

namespace consumer {
void writerThread(const std::string& output_filename, ThreadedQueue& queue);
}
