#include <windows.h>

#include <filesystem>
#include <iostream>
#include <thread>
#include <utility>

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

void print_usage() {
    std::cout << "Usage:   .\\SPSC_Queue.exe <COMPORT> <DURATION (s)> <OUTPUT_FILENAME?>\n";
    std::cout << "Example: .\\SPSC_Queue.exe COM4 120\n";
    std::cout << "Example: .\\SPSC_Queue.exe COM4 120 test_run\n\n";
}
}

// .\SPSC_Queue.exe COM4 120 ?test1
int main(int argc, char* argv[]) {
    try {
        const CliArgs args = parse_cli_args(argc, argv);

        std::filesystem::create_directories("Output");

        std::cout << "Recording " << args.raw_comport << " to " << args.output_filename
                  << " for " << argv[2] << " seconds.\n\n";
        
        KeepAwake display_guard; // RAII destructor guard

        ThreadedQueue queue;
        std::thread producer(producer::reader_thread, args.comport,
                             args.duration, std::ref(queue));
        std::thread consumer(consumer::writer_thread, args.output_filename,
                             std::ref(queue));

        producer.join();
        consumer.join();

        std::cout << "Recording complete!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << "\n" << e.what() << "\n\n";
        print_usage();
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
