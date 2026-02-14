#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

#include "queue/priority_queue.hpp"
#include "thread_pool/thread_pool.hpp"
#include "types.hpp"

namespace dispatcher {

class TaskDispatcher {
private:
    std::shared_ptr<queue::PriorityQueue> priority_queue_;
    std::unique_ptr<thread_pool::ThreadPool> thread_pool_;

public:
    explicit TaskDispatcher(
        std::size_t thread_count,
        queue::PriorityQueue::QueueConfig config = {
            {TaskPriority::High, queue::QueueOptions{.bounded = true, .capacity = 1000}},
            {TaskPriority::Normal, queue::QueueOptions{.bounded = false, .capacity = std::nullopt}}});

    void schedule(TaskPriority priority, std::function<void()> task);
    ~TaskDispatcher();
};

}  // namespace dispatcher
