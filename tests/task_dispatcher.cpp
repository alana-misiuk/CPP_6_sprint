#include <gtest/gtest.h>

#include "task_dispatcher.hpp"

#include <atomic>
#include <future>
#include <mutex>
#include <stdexcept>
#include <vector>

using dispatcher::TaskDispatcher;
using dispatcher::TaskPriority;

TEST(TaskDispatcherTest, ExecutesScheduledTasks) {
    std::atomic<int> counter = 0;
    {
        TaskDispatcher dispatcher(2);
        for (int i = 0; i < 100; ++i) {
            dispatcher.schedule(TaskPriority::Normal, [&counter]() { ++counter; });
        }
    }

    EXPECT_EQ(counter.load(), 100);
}

TEST(TaskDispatcherTest, ExecutesAllTasksBeforeDestruction) {
    std::atomic<int> counter = 0;
    {
        TaskDispatcher dispatcher(4);
        for (int i = 0; i < 500; ++i) {
            dispatcher.schedule(TaskPriority::Normal, [&counter]() { ++counter; });
        }
    }

    EXPECT_EQ(counter.load(), 500);
}

TEST(TaskDispatcherTest, ContinuesExecutionAfterTaskThrows) {
    std::atomic<int> counter = 0;
    {
        TaskDispatcher dispatcher(1);
        dispatcher.schedule(TaskPriority::High, []() { throw std::runtime_error("boom"); });
        dispatcher.schedule(TaskPriority::High, [&counter]() { ++counter; });
    }

    EXPECT_EQ(counter.load(), 1);
}

TEST(TaskDispatcherTest, ExecutesHighPriorityBeforeNormalWhenBothQueued) {
    std::mutex mutex;
    std::vector<char> order;
    std::promise<void> gate_promise;
    auto gate_future = gate_promise.get_future().share();

    {
        TaskDispatcher dispatcher(1);
        dispatcher.schedule(TaskPriority::High, [gate_future]() mutable { gate_future.wait(); });

        for (int i = 0; i < 3; ++i) {
            dispatcher.schedule(TaskPriority::Normal, [&order, &mutex]() {
                std::lock_guard<std::mutex> lock(mutex);
                order.push_back('N');
            });
        }
        for (int i = 0; i < 3; ++i) {
            dispatcher.schedule(TaskPriority::High, [&order, &mutex]() {
                std::lock_guard<std::mutex> lock(mutex);
                order.push_back('H');
            });
        }

        gate_promise.set_value();
    }

    ASSERT_EQ(order.size(), 6U);
    EXPECT_EQ(order, (std::vector<char>{'H', 'H', 'H', 'N', 'N', 'N'}));
}

TEST(TaskDispatcherTest, DefaultConfigHasBoundedHighPriorityQueue) {
    std::promise<void> gate_promise;
    auto gate_future = gate_promise.get_future().share();

    TaskDispatcher dispatcher(1);
    dispatcher.schedule(TaskPriority::High, [gate_future]() mutable { gate_future.wait(); });

    for (int i = 0; i < 1000; ++i) {
        dispatcher.schedule(TaskPriority::High, []() {});
    }

    EXPECT_THROW(dispatcher.schedule(TaskPriority::High, []() {}), std::overflow_error);
    gate_promise.set_value();
}
