#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/**
 * Base for DispatchQueue where all tasks must be processed in a thread by calls
 * to processOne or processAll. All calls are thread safe.
 */
class LocalDispatchQueue {
protected:
    std::queue<std::function<void()>> taskQueue;
    mutable std::mutex m;

public:
    /**
     * Add a task to the queue.
     *
     * @param task the lambda to execute for the task
     */
    void add(std::function<void()> task);

    /**
     * Deque a single task and execute it. This method blocks until the task
     * is complete.
     */
    void processOne();

    /**
     * Dequeue tasks one at a time and execute them sequentially. This method
     * blocks until the task queue is empty.
     */
    void processAll();
};

/**
 * An Apple style Task Dispatch Queue.
 */
class DispatchQueue : protected LocalDispatchQueue {
public:
    /**
     * Dispatch queue type. Serial executes tasks sequentially in a single
     * thread while concurrent executes up to n tasks at once where n is
     * the number of cpu cores as returned from
     * `std::thread::hardware_concurrency()`.
     */
    enum QueueType { Serial, Concurrent };

private:
    std::vector<std::thread> workers;
    std::condition_variable cv;
    std::atomic<bool> running;

    /**
     * Worker thread method. This method takes a task from the queue an executes
     * it.
     *
     * @param id the worker id
     */
    void worker(size_t id);

public:
    /**
     * Create a new `DispatchQueue` with queue `type`.
     * @param type the queue type
     */
    DispatchQueue(QueueType type);

    /**
     * Check if there are tasks waiting for execution.
     */
    bool working() const;

    /**
     * Wait for all currently executing tasks to complete and skip any tasks
     * that have not yet been started.
     */
    void stop();

    /**
     * Wait for all tasks to start execution.
     */
    void wait();

    /**
     * Wait for all tasks to start execution and then wait for their completion.
     */
    void join();

    /**
     * Add a task to the queue.
     *
     * @param task the lambda to execute for the task
     */
    void add(std::function<void()> task);
};