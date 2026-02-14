#include "queue/unbounded_queue.hpp"

#include <functional>
#include <mutex>
#include <queue>
#include <utility>

namespace dispatcher::queue {

UnboundedQueue::UnboundedQueue(int capacity) {
    (void)capacity;
}

void UnboundedQueue::push(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(task));
}

std::optional<std::function<void()>> UnboundedQueue::try_pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return std::nullopt;
    }
    std::function<void()> task = std::move(queue_.front());
    queue_.pop();
    return task;
}

UnboundedQueue::~UnboundedQueue() = default;

} // namespace dispatcher::queue
