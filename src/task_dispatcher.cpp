#include "task_dispatcher.hpp"

#include <utility>

namespace dispatcher {

TaskDispatcher::TaskDispatcher(std::size_t thread_count, queue::PriorityQueue::QueueConfig config)
    : priority_queue_(std::make_shared<queue::PriorityQueue>(config)),
      thread_pool_(std::make_unique<thread_pool::ThreadPool>(priority_queue_, thread_count)) {}

void TaskDispatcher::schedule(TaskPriority priority, std::function<void()> task) {
    priority_queue_->push(priority, std::move(task));
}

TaskDispatcher::~TaskDispatcher() = default;

} // namespace dispatcher
