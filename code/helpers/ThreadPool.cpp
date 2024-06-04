#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    _open_pool(_DEFAULT_THREAD_NUMBER);
}

ThreadPool::ThreadPool(int threads)
{
    _open_pool(threads);
}

ThreadPool::~ThreadPool()
{
    _close_pool();
}

void ThreadPool::_open_pool(int threadNumber)
{
    if (threadNumber >= std::thread::hardware_concurrency())
        throw std::out_of_range("Number of threads exceeds the system capability!");

    _shutdown = false;

    // Create the specified number of threads
    for (int i = 0; i < threadNumber; ++i)
        _threads.emplace_back(std::bind(&ThreadPool::_thread_entry, this, i));
}

void ThreadPool::_close_pool()
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

void ThreadPool::do_job(std::function<void(void)>&& func)
{
    // Place a job on the queue and unblock a thread
    std::unique_lock<std::mutex> lock(_poolMutex);

    _jobs.emplace(std::move(func));
    _poolCondVar.notify_one();
}

void ThreadPool::_thread_entry(int i)
{
    std::function<void(void)> job;

    for (;;)
    {
        {
            std::unique_lock<std::mutex> lock(_poolMutex);

            // If the pool is still working, but there are no jobs -> wait
            if (!_shutdown && _jobs.empty())
                _poolCondVar.wait(lock);

            // True only when the pool is closed -> exit loop and the thread is joined
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