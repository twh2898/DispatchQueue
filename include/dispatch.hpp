#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * An Apple style Task Dispatch Queue.
 */
class DispatchQueue {
public:
    /**
     * Dispatch queue type. Serial executes tasks sequentially in a single
     * thread while concurrent executes up to n tasks at once where n is
     * the number of cpu cores as returned from
     * `std::thread::hardware_concurrency()`.
     */
    enum QueueType { Serial, Concurrent };

private:
    using Fn = std::function<void()>;

    std::vector<std::thread> workers;
    mutable std::mutex m;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::queue<Fn> taskQueue;

    /**
     * Worker thread method. This method takes a task from the queue an executes
     * it.
     *
     * @param id the worker id
     */
    void worker(size_t id) {
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

public:
    /**
     * Create a new `DispatchQueue` with queue `type`.
     * @param type the queue type
     */
    DispatchQueue(QueueType type) {
        workers.emplace_back(&DispatchQueue::worker, this, 0);
        if (type == Concurrent) {
            size_t n = std::thread::hardware_concurrency();
            for (size_t i = 1; i < n; i++) {
                workers.emplace_back(&DispatchQueue::worker, this, i);
            }
        }
    }

    /**
     * Check if there are tasks waiting for execution.
     */
    bool working() const {
        std::unique_lock lk(m);
        return !taskQueue.empty();
    }

    /**
     * Wait for all currently executing tasks to complete and skip any tasks
     * that have not yet been started.
     */
    void stop() {
        running = false;
        cv.notify_all();
        for (auto & worker : workers) {
            worker.join();
        }
    }

    /**
     * Wait for all tasks to start execution and then wait for their completion.
     */
    void join() {
        while (working()) std::this_thread::yield();
        stop();
    }

    /**
     * Add a task to the queue.
     *
     * @param task the lambda to execute for the task
     */
    void add(std::function<void()> task) {
        std::unique_lock lk(m);
        taskQueue.push(task);
        lk.unlock();
        cv.notify_one();
    }
};