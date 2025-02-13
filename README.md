# Implement a Thread Pool

## Problem Description

In high-frequency trading (HFT) systems, every microsecond counts. Your task is to implement a thread pool in C++ that can schedule and execute tasks concurrently with minimal overhead. The implementation must be optimized for ultra-low latency processing in HFT environments.

## Core Requirements

### Thread Pool Creation and Management
- Create a fixed number of worker threads at construction
- Allow clients to submit tasks for execution by worker threads
- Ensure all tasks are completed before the thread pool is destroyed
- Minimize per-task dynamic memory allocation and synchronization overhead

## Implementation Requirements

### 1. Constructor
```cpp
ThreadPool(size_t numThreads)
```
- Initializes the thread pool with specified number of worker threads
- Sets up necessary data structures for task management

### 2. Destructor
- Ensures thread pool stops accepting new tasks
- Guarantees all queued tasks are executed before destruction
- Properly cleans up all worker threads

### 3. Task Submission
```cpp
template<class F, class... Args>
auto enqueue(F&& f, Args&&... args) -> std::future</*return type of F*/>
```
- Accepts a callable object and its arguments
- Schedules task for execution
- Returns a `std::future` representing the task's result

### 4. Optional Shutdown Method
```cpp
void shutdown()
```
- Stops pool from accepting new tasks
- Safely terminates worker threads after current tasks complete

## Technical Constraints

### Thread Safety
- Must be safe for concurrent use by multiple threads
- All operations must maintain consistency and avoid race conditions

### Performance Requirements
- Task submission and execution: O(1) amortized time complexity
- Minimize dynamic memory allocations per task
- Minimize locking overhead (consider lock-free structures)

### Scalability
- Must efficiently handle high-throughput scenarios
- Optimized for lightweight, frequent tasks typical in HFT systems

## Follow-Up Considerations

### 1. Low Latency Optimizations
- Ultra-low latency optimization strategies for HFT environments
- Contention reduction techniques:
  - Per-thread task queues
  - Work stealing algorithms
  - CPU affinity optimization

### 2. Dynamic Resizing
- Lock-free approaches to dynamic thread pool resizing
- Strategies for increasing/decreasing worker threads safely

### 3. Advanced Scheduling
- Task priority implementation approaches
- Handling task dependencies
- Queue management strategies

### 4. Memory Model Considerations
- Usage of memory orderings (`std::memory_order`)
- Impact of memory model choices on HFT performance
- Synchronization primitive optimization

## Example Usage

```cpp
// Create a thread pool with 4 worker threads
ThreadPool pool(4);

// Submit a task and get future
auto future = pool.enqueue([](int x) { 
    return x * x; 
}, 42);

// Get result
int result = future.get(); // Returns 1764

// Optional: Shutdown pool
pool.shutdown();
```

## Performance Benchmarking

Key metrics to measure:
- Task submission latency
- Task execution latency
- Thread context switching overhead
- Memory allocation patterns
- Cache coherency impact

## Best Practices

1. Use lock-free data structures where possible
2. Implement CPU cache-friendly memory layouts
3. Consider NUMA awareness for multi-socket systems
4. Minimize false sharing through proper padding
5. Use appropriate memory barriers and fences
