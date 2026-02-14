#pragma once

#include "queue/priority_queue.hpp"

#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

namespace dispatcher::thread_pool {

class ThreadPool {
private:
    std::shared_ptr<queue::PriorityQueue> priority_queue_;
    std::vector<std::thread> workers_;

    void worker_loop();

public:
    ThreadPool(std::shared_ptr<queue::PriorityQueue> priority_queue, std::size_t thread_count);
    ~ThreadPool();
};

} // namespace dispatcher::thread_pool
