#include <gtest/gtest.h>

#include "queue/bounded_queue.hpp"

#include <atomic>
#include <stdexcept>
#include <vector>

using dispatcher::queue::BoundedQueue;

TEST(BoundedQueueTest, ConstructorThrowsOnNonPositiveCapacity) {
    EXPECT_THROW(BoundedQueue(0), std::invalid_argument);
}

TEST(BoundedQueueTest, PushAndPopSingleTask) {
    BoundedQueue queue(2);
    std::atomic<int> value = 0;

    queue.push([&value]() { ++value; });
    auto task = queue.try_pop();

    ASSERT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(value.load(), 1);
}

TEST(BoundedQueueTest, PreservesFifoOrder) {
    BoundedQueue queue(3);
    std::vector<int> result;

    queue.push([&result]() { result.push_back(1); });
    queue.push([&result]() { result.push_back(2); });

    auto first = queue.try_pop();
    auto second = queue.try_pop();

    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    (*first)();
    (*second)();
    EXPECT_EQ(result, (std::vector<int>{1, 2}));
}

TEST(BoundedQueueTest, TryPopReturnsNulloptOnEmptyQueue) {
    BoundedQueue queue(1);
    EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(BoundedQueueTest, PushThrowsWhenQueueIsFull) {
    BoundedQueue queue(1);
    queue.push([]() {});
    EXPECT_THROW(queue.push([]() {}), std::overflow_error);
}
