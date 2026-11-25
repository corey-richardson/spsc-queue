#pragma once
#include <string>

#include "ThreadedQueue.h"

namespace consumer {
void writer_thread(const std::string& output_filename, ThreadedQueue& queue);
}
