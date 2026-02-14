#include "queue/bounded_queue.hpp"

#include <stdexcept>
#include <utility>

namespace dispatcher::queue {

BoundedQueue::BoundedQueue(int capacity) : capacity_(capacity) {
    if (capacity_ <= 0) {
        throw std::invalid_argument("BoundedQueue capacity must be positive");
    }
}

void BoundedQueue::push(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (static_cast<int>(queue_.size()) >= capacity_) {
        throw std::overflow_error("BoundedQueue is full");
    }
    queue_.push(std::move(task));
}

std::optional<std::function<void()>> BoundedQueue::try_pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return std::nullopt;
    }
    std::function<void()> task = std::move(queue_.front());
    queue_.pop();
    return task;
}

BoundedQueue::~BoundedQueue() = default;

} // namespace dispatcher::queue
