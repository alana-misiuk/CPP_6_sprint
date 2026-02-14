#pragma once
#include "queue/bounded_queue.hpp"
#include "queue/unbounded_queue.hpp"
#include "types.hpp"

#include <atomic>
#include <condition_variable>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace dispatcher::queue {

class PriorityQueue {
public:
    using QueueConfig = std::unordered_map<TaskPriority, QueueOptions>;

private:
    std::map<TaskPriority, std::shared_ptr<IQueue>> queues_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool is_shutdown_ = false;
    std::size_t size_ = 0;

public:
    explicit PriorityQueue(const QueueConfig &config);

    void push(TaskPriority priority, std::function<void()> task);
    // block on pop until shutdown is called
    // after that return std::nullopt on empty queue
    std::optional<std::function<void()>> pop();

    void shutdown();

    ~PriorityQueue();
};

}  // namespace dispatcher::queue