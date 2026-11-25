#include "Consumer.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Config.h"
#include "Message.h"

void consumer::writer_thread(const std::string& output_filename,
                            ThreadedQueue& queue) {
    std::ofstream file(output_filename, std::ios::out);
    if (!file) {
        throw std::runtime_error("Failed to open output file.");
    }

    file << "SyncByte,Byte1,Byte2,SensorValue1,Byte3,Byte4,SensorValue2,CS\n";

    Message message {};

    auto last_display_time = std::chrono::steady_clock::now();

    while (queue.pop(message)) {
        file << message;

        auto now = std::chrono::steady_clock::now();
        if (now - last_display_time >= config::PRINT_SUBSAMPLE_RATE) {
            std::cout << message;
            last_display_time = now;
        }
    }

    file.flush();
    file.close();
}
