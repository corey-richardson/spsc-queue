#include <windows.h>

#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "CliArgs.h"
#include "Consumer.h"
#include "Producer.h"
#include "ThreadedQueue.h"

namespace {
struct KeepAwake {
    KeepAwake() {
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
    }

    ~KeepAwake() { 
        SetThreadExecutionState(ES_CONTINUOUS);
    }

    KeepAwake(const KeepAwake&) = delete;
    KeepAwake& operator=(const KeepAwake&) = delete;
};

// Prevents std::terminate on exceptions
// JoinThreads joins joinable threads during stack unwinding
// Declare ThreadedQueue queue; before JoinThreads!
struct JoinThreads {
    std::thread& producer;
    std::thread& consumer;

    JoinThreads(std::thread& p, std::thread& c) : producer(p), consumer(c) {}

    ~JoinThreads() {
        if (producer.joinable()) {
            producer.join();
        }
        if (consumer.joinable()) {
            consumer.join();
        }
    }

    JoinThreads(const JoinThreads&) = delete;
    JoinThreads& operator=(const JoinThreads&) = delete;
};

void print_usage() {
    std::cerr << "Usage:   .\\SPSC_Queue.exe <COMPORT> <DURATION (s)> <OUTPUT_FILENAME?>\n";
    std::cerr << "Example: .\\SPSC_Queue.exe COM4 120\n";
    std::cerr << "Example: .\\SPSC_Queue.exe COM4 120 test_run\n\n";
}
}

// .\SPSC_Queue.exe COM4 120 ?test1
int main(int argc, char* argv[]) {
    try {
        const CliArgs args = parse_cli_args(argc, argv);

        std::filesystem::create_directories("Output");

        std::cout << "Recording " << args.raw_comport << " to " 
                  << args.output_filename << " for " 
                  << args.duration.count() << " seconds.\n\n";
        
        KeepAwake display_guard; // RAII destructor guard

        ThreadedQueue queue;
        std::thread producer(producer::reader_thread, args.comport,
                             args.duration, std::ref(queue));
        std::thread consumer(consumer::writer_thread, args.output_filename,
                             std::ref(queue));

        JoinThreads join_guard { producer, consumer };

        producer.join();
        consumer.join();

        std::cout << "Recording complete!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "\n" << e.what() << "\n\n";
        print_usage();
        return 1;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to create 'Output' directory: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred.\n";
        return 1;
    }

    return 0;
}
