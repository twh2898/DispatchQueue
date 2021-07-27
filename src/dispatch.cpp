#include "dispatch.hpp"

void DispatchQueue::worker(size_t id) {
    while (running) {
        std::unique_lock lk(m);
        cv.wait(lk, [&]() {
            return !running || !taskQueue.empty();
        });

        if (!running)
            break;

        auto task = taskQueue.front();
        taskQueue.pop();
        lk.unlock();
        task();
    }
}

DispatchQueue::DispatchQueue(QueueType type) {
    workers.emplace_back(&DispatchQueue::worker, this, 0);
    if (type == Concurrent) {
        size_t n = std::thread::hardware_concurrency();
        for (size_t i = 1; i < n; i++) {
            workers.emplace_back(&DispatchQueue::worker, this, i);
        }
    }
}

bool DispatchQueue::working() const {
    std::unique_lock lk(m);
    return !taskQueue.empty();
}

void DispatchQueue::stop() {
    running = false;
    cv.notify_all();
    for (auto & worker : workers) {
        worker.join();
    }
}

void DispatchQueue::join() {
    while (working()) std::this_thread::yield();
    stop();
}

void DispatchQueue::add(std::function<void()> task) {
    std::unique_lock lk(m);
    taskQueue.push(task);
    lk.unlock();
    cv.notify_one();
}