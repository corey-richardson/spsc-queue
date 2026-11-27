#pragma once

#include <windows.h>

#include <thread>

// RAII abstractions

namespace guards {

class KeepAwake {
// Class could be a struct, chose class to keep consistent with JoinThreads
public:
    KeepAwake() {
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
    }

    ~KeepAwake() { SetThreadExecutionState(ES_CONTINUOUS); }

    KeepAwake(const KeepAwake&) = delete;
    KeepAwake& operator=(const KeepAwake&) = delete;
};

class JoinThreads {
public:
    JoinThreads(std::thread& p, std::thread& c) : producer_(p), consumer_(c) {}

    ~JoinThreads() {
        if (producer_.joinable()) {
            producer_.join();
        }
        if (consumer_.joinable()) {
            consumer_.join();
        }
    }

    JoinThreads(const JoinThreads&) = delete;
    JoinThreads& operator=(const JoinThreads&) = delete;

   private:
    std::thread& producer_;
    std::thread& consumer_;
};
}
