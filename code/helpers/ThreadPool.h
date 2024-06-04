#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <list>
#include <functional>

class ThreadPoolDisplay;

class ThreadPool
{
    friend ThreadPoolDisplay;

protected:
    bool                                        _shutdown;
    std::mutex                                  _poolMutex;
    std::condition_variable                     _poolCondVar;
    std::list<std::thread>                      _threads;
    std::queue<std::function<void(void)>>       _jobs;

    static constexpr int _DEFAULT_THREAD_NUMBER = 2;

public:
    ThreadPool();
    ThreadPool(int threadNumber);
    ~ThreadPool();

    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;

    void do_job(std::function<void(void)>&& func);

protected:
    void _open_pool(int threadNumber);
    void _close_pool();
    void _thread_entry(int i);
};