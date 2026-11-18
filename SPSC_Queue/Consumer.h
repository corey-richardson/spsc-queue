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
void writer_thread(const std::string& output_filename, ThreadedQueue& queue);
}
