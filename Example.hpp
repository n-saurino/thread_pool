#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(); // Non-templated function

    void shutdown();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;

    void workerThread();
};

// Constructor: Creates a fixed number of worker threads
inline ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

// Destructor: Ensures all tasks finish before shutting down
ThreadPool::~ThreadPool() {
    shutdown();
}

// Enqueue a "HelloWorld" task (no templates needed)
inline void ThreadPool::enqueue() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.emplace(HelloWorld); // Push HelloWorld into the queue
    }
    condition.notify_one(); // Wake up one worker thread
}

// Worker thread function
inline void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop.load() || !tasks.empty(); });

            if (stop.load() && tasks.empty()) return;

            task = std::move(tasks.front());
            tasks.pop();
        }

        task(); // Execute the task
    }
}

// Shutdown function: Ensures all threads exit gracefully
inline void ThreadPool::shutdown() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop.store(true);
    }

    condition.notify_all();
    for (std::thread& worker : workers) {
        if (worker.joinable()) worker.join();
    }
}
