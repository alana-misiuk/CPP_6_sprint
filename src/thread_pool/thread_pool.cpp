#include "thread_pool/thread_pool.hpp"

#include <algorithm>
#include <utility>

namespace dispatcher::thread_pool {

ThreadPool::ThreadPool(std::shared_ptr<queue::PriorityQueue> priority_queue, std::size_t thread_count)
    : priority_queue_(std::move(priority_queue)) {
    const std::size_t workers_count = std::max<std::size_t>(1, thread_count);
    workers_.reserve(workers_count);
    for (std::size_t i = 0; i < workers_count; ++i)
        workers_.emplace_back([this]() { worker_loop(); });
}

void ThreadPool::worker_loop() {
    while (true) {
        auto task = priority_queue_->pop();
        if (!task.has_value())
            return;

        (*task)();
    }
}

ThreadPool::~ThreadPool() {
    priority_queue_->shutdown();
    for (auto &worker : workers_) {
        if (worker.joinable())
            worker.join();
    }
}

}  // namespace dispatcher::thread_pool
