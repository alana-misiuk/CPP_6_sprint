#include <gtest/gtest.h>

#include "queue/unbounded_queue.hpp"

#include <atomic>
#include <vector>

using dispatcher::queue::UnboundedQueue;

TEST(UnboundedQueueTest, PushAndPopSingleTask) {
    UnboundedQueue queue(0);
    std::atomic<int> value = 0;

    queue.push([&value]() { ++value; });
    auto task = queue.try_pop();

    ASSERT_TRUE(task.has_value());
    (*task)();
    EXPECT_EQ(value.load(), 1);
}

TEST(UnboundedQueueTest, PreservesFifoOrder) {
    UnboundedQueue queue(0);
    std::vector<int> result;

    queue.push([&result]() { result.push_back(1); });
    queue.push([&result]() { result.push_back(2); });
    queue.push([&result]() { result.push_back(3); });

    auto first = queue.try_pop();
    auto second = queue.try_pop();
    auto third = queue.try_pop();

    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    ASSERT_TRUE(third.has_value());

    (*first)();
    (*second)();
    (*third)();
    EXPECT_EQ(result, (std::vector<int>{1, 2, 3}));
}

TEST(UnboundedQueueTest, TryPopReturnsNulloptOnEmptyQueue) {
    UnboundedQueue queue(0);
    EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(UnboundedQueueTest, AcceptsManyTasks) {
    UnboundedQueue queue(0);
    constexpr int kTasksCount = 42;
    for (int i = 0; i < kTasksCount; ++i) {
        queue.push([]() {});
    }

    int popped = 0;
    while (queue.try_pop().has_value()) {
        ++popped;
    }
    EXPECT_EQ(popped, kTasksCount);
}

TEST(UnboundedQueueTest, IgnoresCapacityArgument) {
    UnboundedQueue queue(-5);
    queue.push([]() {});
    EXPECT_TRUE(queue.try_pop().has_value());
}
