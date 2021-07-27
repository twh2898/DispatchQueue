#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class DispatchQueue {
public:
    enum QueueType { Serial, Concurrent };

private:
    using Fn = std::function<void()>;

    std::vector<std::thread> workers;
    mutable std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::queue<Fn> taskQueue;

    void worker(size_t id);

public:
    DispatchQueue(QueueType type);

    bool working() const;

    void stop();

    void join();

    void add(std::function<void()> task);
};