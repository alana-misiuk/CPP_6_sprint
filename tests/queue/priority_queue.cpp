#include <gtest/gtest.h>

#include "queue/priority_queue.hpp"

#include <chrono>
#include <future>
#include <mutex>
#include <vector>

using dispatcher::TaskPriority;
using dispatcher::queue::PriorityQueue;
using dispatcher::queue::QueueOptions;

TEST(PriorityQueueTest, PopsHighPriorityBeforeNormal) {
    PriorityQueue queue({
        {TaskPriority::High, QueueOptions{.bounded = false, .capacity = std::nullopt}},
        {TaskPriority::Normal, QueueOptions{.bounded = false, .capacity = std::nullopt}},
    });

    std::vector<int> order;
    queue.push(TaskPriority::Normal, [&order]() { order.push_back(2); });
    queue.push(TaskPriority::High, [&order]() { order.push_back(1); });

    auto first = queue.pop();
    auto second = queue.pop();

    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    (*first)();
    (*second)();
    EXPECT_EQ(order, (std::vector<int>{1, 2}));
    queue.shutdown();
}

TEST(PriorityQueueTest, PreservesFifoInsideSamePriority) {
    PriorityQueue queue({
        {TaskPriority::High, QueueOptions{.bounded = false, .capacity = std::nullopt}},
        {TaskPriority::Normal, QueueOptions{.bounded = false, .capacity = std::nullopt}},
    });

    std::vector<int> order;
    queue.push(TaskPriority::High, [&order]() { order.push_back(1); });
    queue.push(TaskPriority::High, [&order]() { order.push_back(2); });

    auto first = queue.pop();
    auto second = queue.pop();

    ASSERT_TRUE(first.has_value());
    ASSERT_TRUE(second.has_value());
    (*first)();
    (*second)();

    EXPECT_EQ(order, (std::vector<int>{1, 2}));
    queue.shutdown();
}

TEST(PriorityQueueTest, PopBlocksUntilTaskArrives) {
    PriorityQueue queue({
        {TaskPriority::High, QueueOptions{.bounded = false, .capacity = std::nullopt}},
        {TaskPriority::Normal, QueueOptions{.bounded = false, .capacity = std::nullopt}},
    });

    auto future = std::async(std::launch::async, [&queue]() { return queue.pop(); });

    EXPECT_EQ(future.wait_for(std::chrono::milliseconds(50)), std::future_status::timeout);

    queue.push(TaskPriority::Normal, []() {});
    auto result = future.get();
    EXPECT_TRUE(result.has_value());
    queue.shutdown();
}

TEST(PriorityQueueTest, ShutdownUnblocksPopAndReturnsNulloptWhenEmpty) {
    PriorityQueue queue({
        {TaskPriority::High, QueueOptions{.bounded = false, .capacity = std::nullopt}},
        {TaskPriority::Normal, QueueOptions{.bounded = false, .capacity = std::nullopt}},
    });

    auto future = std::async(std::launch::async, [&queue]() { return queue.pop(); });
    EXPECT_EQ(future.wait_for(std::chrono::milliseconds(50)), std::future_status::timeout);

    queue.shutdown();
    auto result = future.get();
    EXPECT_FALSE(result.has_value());
}
