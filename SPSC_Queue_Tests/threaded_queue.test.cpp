#include "pch.h"

#include <gtest/gtest.h>

#include "Message.h"
#include "ThreadedQueue.h"

namespace {
Message create_sample_message(uint8_t byte2, uint8_t checksum) {
    Message message {};
    message.bytes = { 0x55, 0x00, byte2, 0x00, 0x00 };
    message.checksum = checksum;

    return message;
}
}

TEST(ThreadedQueueTests, PreservesFifoOrder) { 
    ThreadedQueue queue;

    const Message first = create_sample_message(1, 169);
    const Message second = create_sample_message(2, 168);
    const Message third = create_sample_message(3, 167);

    queue.push(first);
    queue.push(second);
    queue.push(third);

    Message out {};

    ASSERT_TRUE(queue.pop(out));
    EXPECT_EQ(out.bytes, first.bytes);
    EXPECT_EQ(out.checksum, first.checksum);

    ASSERT_TRUE(queue.pop(out));
    EXPECT_EQ(out.bytes, second.bytes);
    EXPECT_EQ(out.checksum, second.checksum);

    ASSERT_TRUE(queue.pop(out));
    EXPECT_EQ(out.bytes, third.bytes);
    EXPECT_EQ(out.checksum, third.checksum);
}

TEST(ThreadedQueueTests, ReturnsFalseWhenEmptyAndIsFinished) {
    ThreadedQueue queue;
    queue.set_is_finished();

    Message out {};
    EXPECT_FALSE(queue.pop(out));
}

TEST(ThreadedQueueTests, DrainsQueuedItemsAfterSetIsFinished) {
    ThreadedQueue queue;
    queue.push(create_sample_message(10, 160));
    queue.push(create_sample_message(20, 150));
    queue.set_is_finished();

    Message out {};

    ASSERT_TRUE(queue.pop(out));
    ASSERT_TRUE(queue.pop(out));
    EXPECT_FALSE(queue.pop(out));
}

// TODO: Create concurrency tests
