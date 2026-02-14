#include "queue/priority_queue.hpp"

#include <array>
#include <utility>

namespace dispatcher::queue {

PriorityQueue::PriorityQueue(const QueueConfig &config) {
    static constexpr std::array<TaskPriority, 2> kRequiredPriorities = {
        TaskPriority::High,
        TaskPriority::Normal,
    };

    for (const TaskPriority priority : kRequiredPriorities) {
        const auto options_it = config.find(priority);
        if (options_it == config.end()) {
            throw std::invalid_argument("Missing queue configuration for priority");
        }

        const QueueOptions &options = options_it->second;
        if (options.bounded) {
            if (!options.capacity.has_value()) {
                throw std::invalid_argument("Bounded queue requires capacity");
            }
            queues_.emplace(priority, std::make_shared<BoundedQueue>(*options.capacity));
        } else {
            queues_.emplace(priority, std::make_shared<UnboundedQueue>(options.capacity.value_or(0)));
        }
    }
}

void PriorityQueue::push(TaskPriority priority, std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (is_shutdown_)
        throw std::runtime_error("PriorityQueue is shut down");

    const auto queue_it = queues_.find(priority);
    if (queue_it == queues_.end())
        throw std::invalid_argument("Unknown task priority");

    queue_it->second->push(std::move(task));
    ++size_;
    condition_.notify_one();
}

std::optional<std::function<void()>> PriorityQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (true) {
        condition_.wait(lock, [this]() { return is_shutdown_ || size_ > 0; });

        if (size_ == 0 && is_shutdown_)
            return std::nullopt;

        for (const auto &[_, queue] : queues_) {
            if (auto task = queue->try_pop(); task.has_value()) {
                --size_;
                return task;
            }
        }
    }
}

void PriorityQueue::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    is_shutdown_ = true;
    condition_.notify_all();
}

PriorityQueue::~PriorityQueue() = default;

}  // namespace dispatcher::queue
