#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <list>
#include <functional>
#include "Config.hpp"


class ThreadPoolDisplay;

template<int NThread = 2>
class ThreadPool
{
    friend ThreadPoolDisplay;

    static_assert(NThread < MACHINE_NUM_PROCESSORS, "Number of threads exceeds the system capability!");

protected:
    bool                                        _shutdown;
    std::thread                                 _threads[NThread];
    std::mutex                                  _poolMutex;
    std::condition_variable                     _poolCondVar;
    std::queue<std::function<void(void)>>       _jobs;


public:
    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool& other)             = delete;
    ThreadPool& operator=(const ThreadPool& other)  = delete;

    void do_job(std::function<void(void)>&& func);

protected:
    void _open_pool();
    void _close_pool();
    void _thread_work();
};


// Implementation ===========================================================

template<int NThread>
ThreadPool<NThread>::ThreadPool()
{
    _open_pool();
}

template<int NThread>
ThreadPool<NThread>::~ThreadPool()
{
    _close_pool();
}

template<int NThread>
void ThreadPool<NThread>::_open_pool()
{
    _shutdown = false;

    // Create the specified number of threads
    for (auto& t : _threads)
        t = std::thread(std::bind(ThreadPool<NThread>::_thread_work, this));
}

template<int NThread>
void ThreadPool<NThread>::_close_pool()
{
    // Notify all threads to stop waiting for job
    // Threads will exit the loop and will join here
    {
        // Unblock any threads and tell them to stop
        std::unique_lock<std::mutex> lock(_poolMutex);

        _shutdown = true;
        _poolCondVar.notify_all();
    }

    // Wait for all threads to stop
    for (auto& thread : _threads)
        thread.join();
}

template<int NThread>
void ThreadPool<NThread>::do_job(std::function<void(void)>&& func)
{
    // Place a job on the queue and unblock a thread
    std::unique_lock<std::mutex> lock(_poolMutex);

    _jobs.emplace(std::move(func));
    _poolCondVar.notify_one();
}

template<int NThread>
void ThreadPool<NThread>::_thread_work()
{
    std::function<void(void)> job;

    for (;;)
    {
        {
            std::unique_lock<std::mutex> lock(_poolMutex);

            // If the pool is still working, but there are no jobs -> wait
            if (!_shutdown && _jobs.empty())
                _poolCondVar.wait(lock);

            // True only when the pool is closed -> exit loop and this thread is joined
            if (_jobs.empty())
                return;

            // Assign job to thread from queue
            job = std::move(_jobs.front());
            _jobs.pop();
        }

        // Do the job without holding any locks
        job();
    }
}