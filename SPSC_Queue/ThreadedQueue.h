#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

#include "Message.h"

class ThreadedQueue {
   private:
    std::queue<Message> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool is_finished_ = false;

   public:
    void push(Message message);
    bool pop(Message& message);
    void set_is_finished();
};
