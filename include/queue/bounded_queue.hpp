#pragma once
#include "queue/queue.hpp"

#include <mutex>
#include <queue>

namespace dispatcher::queue {

class BoundedQueue : public IQueue {
private:
    int capacity_;
    std::queue<std::function<void()>> queue_;
    std::mutex mutex_;

public:
    explicit BoundedQueue(int capacity);

    void push(std::function<void()> task) override;

    std::optional<std::function<void()>> try_pop() override;

    ~BoundedQueue() override;
};

}  // namespace dispatcher::queue
