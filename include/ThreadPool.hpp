#pragma once
#include <deque>
#include <thread>
#include <future>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <vector>

class ThreadPool{
public:
    explicit ThreadPool(int num_thread){

    }

    ~Threadpool(){

    }

    template<typename F, typename... Args>
    auto Push(F&& func, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type>{

    }

    void Shutdown(){

    }
private:
    std::mutex q_mtx_{};
    std::vector<std::thread> threads_{};
    std::deque<std::function<void()>> task_q_{};
    std::condition_variable cv_{};
    std::atomic<bool> stop_{};
    
    // function that threads run to cycle, checking the queue for new tasks
    // and executing them
    void WorkerThread(){
        
    }
};