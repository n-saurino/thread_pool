#include <gtest/gtest.h>
#include "ThreadPool.hpp"
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>

// **Fixture Class** for setting up and tearing down the ThreadPool
class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<ThreadPool>(4); // 4 worker threads
    }

    void TearDown() override {
        pool->Shutdown(); // Ensure shutdown after each test
    }

    std::unique_ptr<ThreadPool> pool;
};

// ✅ **Test 1: Basic Task Execution**
TEST_F(ThreadPoolTest, BasicTaskExecution) {
    std::atomic<int> counter{0};

    auto task = [&counter]() { counter.fetch_add(1, std::memory_order_relaxed); };
    
    pool->Push(task);
    pool->Push(task);
    pool->Push(task);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Allow threads to execute

    EXPECT_EQ(counter.load(), 3);
}

// ✅ **Test 2: Multiple Threads Participate**
TEST_F(ThreadPoolTest, MultipleThreadsUtilized) {
    std::atomic<int> counter{0};
    std::set<std::thread::id> threadIds;

    auto task = [&counter, &threadIds]() {
        threadIds.insert(std::this_thread::get_id());
        counter.fetch_add(1, std::memory_order_relaxed);
    };

    for (int i = 0; i < 10; ++i) {
        pool->Push(task);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Allow threads to execute

    EXPECT_GE(threadIds.size(), 2); // At least two different threads should execute tasks
    EXPECT_EQ(counter.load(), 10);
}

// ✅ **Test 3: Ensure Tasks Complete Before Shutdown**
TEST_F(ThreadPoolTest, TasksCompleteBeforeShutdown) {

    std::atomic<int> counter{0};

    auto task = [&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate workload
        counter.fetch_add(1, std::memory_order_relaxed);
    };

    for (int i = 0; i < 5; ++i) {
        pool->Push(task);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pool->Shutdown();

    EXPECT_EQ(counter.load(), 5); // Ensure all tasks completed before shutdown
}

// ✅ **Test 4: No Execution After Shutdown**
TEST_F(ThreadPoolTest, NoExecutionAfterShutdown) {
    std::atomic<int> counter{0};

    auto task = [&counter]() { counter.fetch_add(1, std::memory_order_relaxed); };

    pool->Shutdown(); // Shut down first
    pool->Push(task); // Try to add a task after shutdown

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(), 0); // No tasks should execute after shutdown
}

// ✅ **Test 5: Stress Test with Many Tasks**
TEST_F(ThreadPoolTest, StressTest) {
    std::atomic<int> counter{0};

    auto task = [&counter]() { counter.fetch_add(1, std::memory_order_relaxed); };

    for (int i = 0; i < 1000; ++i) {
        pool->Push(task);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // Let tasks execute

    EXPECT_EQ(counter.load(), 1000);
}

// ✅ **Test 6: Tasks Execute in Parallel**
TEST_F(ThreadPoolTest, TasksExecuteInParallel) {
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;

    auto task = [&counter]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Simulate work
        counter.fetch_add(1, std::memory_order_relaxed);
    };

    for (int i = 0; i < 10; ++i) {
        pool->Push(task);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Allow time for execution

    EXPECT_EQ(counter.load(), 10);
}