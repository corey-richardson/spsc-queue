#include <chrono>
#include <format>
#include <iostream>
#include <thread>

#include "Config.h"
#include "Consumer.h"
#include "Producer.h"
#include "ThreadedQueue.h"

std::string get_timestamped_filename() {
    auto now = std::chrono::floor<std::chrono::seconds>(
        std::chrono::current_zone()->to_local(
            std::chrono::system_clock::now()));

    return std::format("{}_{:%Y-%m-%d_%H-%M-%S}.csv", config::FILENAME_PREFIX,
                       now);
}

// .\SPSC_Queue.exe COM4 120 ?test1
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "\nMissing required arguments!\n\n";
        std::cout << "Usage:   .\\SPSC_Queue.exe <COMPORT> <DURATION "
                     "(s)> <OUTPUT_FILENAME?>\n";
        std::cout << "Example: .\\SPSC_Queue.exe COM4 120\n";
        std::cout << "Example: .\\SPSC_Queue.exe COM4 120 test_run\n\n";

        return -1;
    }

    const std::string comport = "\\\\.\\" + std::string(argv[1]);
    const std::chrono::seconds duration(atoi(argv[2]));

    const std::string output_filename = (argc > 3) 
        ? (std::string(argv[3]) + ".csv")
        : get_timestamped_filename();

    std::cout << "Recording " << argv[1] << " to " << output_filename << " for "
              << argv[2] << " seconds.\n\n";
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

    ThreadedQueue queue;
    std::thread producer(producer::reader_thread, comport, duration,
                         std::ref(queue));
    std::thread consumer(consumer::writer_thread, output_filename,
                         std::ref(queue));

    producer.join();
    consumer.join();

    std::cout << "Recording complete!" << std::endl;
    SetThreadExecutionState(ES_CONTINUOUS);

    return 0;
}
