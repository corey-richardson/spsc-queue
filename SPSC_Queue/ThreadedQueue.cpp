#include "ThreadedQueue.h"

// Used by producer thread
void ThreadedQueue::push(Message message) {
    // Uses scope-guarding to release the lock through RAII priciple
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(message));
    }
    // ^ Lock is released here, before condition variable notifies
    // the consumer thread that the resource is free, minimising lock time.
    cv_.notify_one();
}

// Used by consumer thread
bool ThreadedQueue::pop(Message& message) {
    // Wait for lock to be free, and acquire
    std::unique_lock<std::mutex> lock(mutex_);

    // If lambda function returns false, release the lock
    // On notification, reacquire the lock BEFORE checking the lambda conditions
    cv_.wait(lock, [this] { return !queue_.empty() || is_finished_; });

    if (queue_.empty() && is_finished_) {
        return false;
        // Lock goes out of scope, RAII
    }

    // std::move avoids a copy operation by transferring ownership from
    // queue->message
    message = std::move(queue_.front());
    queue_.pop();

    return true;
    // Lock goes out of scope, RAII
}

// Used by the producer thread to indicate it is no longer
// recording messages (duration elapsed).
void ThreadedQueue::set_is_finished() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        is_finished_ = true;
    }
    cv_.notify_all();
}
