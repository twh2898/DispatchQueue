#include "DispatchQueue.hpp"

void LocalDispatchQueue::add(std::function<void()> task) {
    std::unique_lock lk(m);
    taskQueue.push(task);
    lk.unlock();
}

void LocalDispatchQueue::processOne() {
    std::unique_lock lk(m);
    if (!taskQueue.empty()) {
        auto task = taskQueue.front();
        taskQueue.pop();
        lk.unlock();
        task();
        lk.lock();
    }
}

void LocalDispatchQueue::processAll() {
    std::unique_lock lk(m);
    while (!taskQueue.empty()) {
        auto task = taskQueue.front();
        taskQueue.pop();
        lk.unlock();
        task();
        lk.lock();
    }
}

void DispatchQueue::worker(size_t id) {
    while (running) {
        {
            std::unique_lock lk(m);
            cv.wait(lk, [&]() {
                return !running || !taskQueue.empty();
            });

            if (!running)
                break;
        }
        processOne();
    }
}

DispatchQueue::DispatchQueue(QueueType type) : running(true) {
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

void DispatchQueue::wait() {
    while (working()) std::this_thread::yield();
}

void DispatchQueue::join() {
    wait();
    stop();
}

void DispatchQueue::add(std::function<void()> task) {
    LocalDispatchQueue::add(task);
    cv.notify_one();
}