#pragma once
#include <deque>
#include <thread>
#include <future>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <vector>

/*
    Non-templated solution for proof of concept. 
    Generic programming makes this so much messier.
*/

class ThreadPool{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(int num_threads): stop_{false}{
        threads_.reserve(num_threads);
        for(int i{}; i < num_threads; ++i){
            threads_.emplace_back(&ThreadPool::WorkerThread, this);
        }
    }

    void Shutdown(){
        {
            std::lock_guard<std::mutex> lg{q_mtx_};
            stop_.store(true);   
        }

        cv_.notify_all();
        for(std::thread& worker : threads_){
            if(worker.joinable()){
                worker.join();
            }
        } 
    }

    ~ThreadPool(){
        Shutdown();        
    }

    void Push(Task task){
        {
            std::unique_lock<std::mutex> ul{q_mtx_};
            task_q_.emplace_back(std::move(task));
        }
        
        cv_.notify_one();
    } 
   
private:
    std::mutex q_mtx_{};
    std::vector<std::thread> threads_{};
    std::deque<Task> task_q_{};
    std::condition_variable cv_{};
    std::atomic<bool> stop_{};
    
    // function that threads run to cycle, checking the queue for new tasks
    // and executing them
    void WorkerThread(){

        while(!stop_){
            Task curr_task{};
            {
                std::unique_lock<std::mutex> ul(q_mtx_);
                cv_.wait(ul, [this](){ return stop_.load() || !task_q_.empty();});

                if(stop_.load() && task_q_.empty()){
                    return;
                }
                
                curr_task = std::move(task_q_.front());
                task_q_.pop_front();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            curr_task();
        }
    }
};