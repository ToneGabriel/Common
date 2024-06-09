#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <functional>
#include "Common.h"


template<size_t NThread = 2>
class ThreadPool
{
public:
    static_assert(NThread < MACHINE_NUM_PROCESSORS, "Number of threads exceeds the system capability!");

    using JobQueue_t = std::queue<std::function<void(void)>>;

private:
    JobQueue_t              _jobs;
    std::thread             _workerThreads[NThread];
    std::mutex              _poolMtx;
    std::condition_variable _poolCV;
    std::atomic_size_t      _jobsDone;
    bool                    _shutdown;

public:

    ThreadPool()
    {
        _open_pool();
    }

    ThreadPool(const JobQueue_t& jobs)
        : _jobs(jobs)
    {
        _open_pool();
        _notify_at_startup();
    }

    ThreadPool(JobQueue_t&& jobs)
        : _jobs(std::move(jobs))
    {
        _open_pool();
        _notify_at_startup();
    }

    ~ThreadPool()
    {
        _close_pool();
    }

    ThreadPool(const ThreadPool& other)             = delete;
    ThreadPool& operator=(const ThreadPool& other)  = delete;

    void do_job(const std::function<void(void)>& func)
    {
        // Copy a job on the queue and unblock a thread
        std::unique_lock<std::mutex> lock(_poolMtx);

        _jobs.emplace(func);
        _poolCV.notify_one();
    }

    void do_job(std::function<void(void)>&& func)
    {
        // Move a job on the queue and unblock a thread
        std::unique_lock<std::mutex> lock(_poolMtx);

        _jobs.emplace(std::move(func));
        _poolCV.notify_one();
    }

    size_t jobs_done() const
    {
        return _jobsDone;
    }

private:

    void _open_pool()
    {
        _shutdown = false;
        _jobsDone = 0;

        // Create the specified number of threads
        for (auto& t : _workerThreads)
            t = std::thread(std::bind(ThreadPool<NThread>::_worker_thread, this));
    }

    void _close_pool()
    {
        // Notify all threads to stop waiting for job
        // Threads will exit the loop and will join here

        {   // empty scope start -> lock
            std::unique_lock<std::mutex> lock(_poolMtx);

            _shutdown = true;
            _poolCV.notify_all();
        }   // empty scope end -> unlock

        for (auto& t : _workerThreads)
            t.join();
    }

    void _notify_at_startup()
    {
        std::unique_lock<std::mutex> lock(_poolMtx);
        size_t startupSize = _jobs.size();

        if (startupSize < NThread)
            for (size_t i = 0; i < startupSize; ++i)
                _poolCV.notify_one();
        else
            _poolCV.notify_all();
    }

    void _worker_thread()
    {
        std::function<void(void)> job;

        for (;;)
        {
            {   // empty scope start -> mutex lock and job decision
                std::unique_lock<std::mutex> lock(_poolMtx);

                // If the pool is still working, but there are no jobs -> wait
                if (!_shutdown && _jobs.empty())
                    _poolCV.wait(lock);

                // True only when the pool is closed -> exit loop and this thread is joined
                if (_jobs.empty())
                    return;

                // Assign job to thread from queue
                job = std::move(_jobs.front());
                _jobs.pop();
            }   // empty scope end -> unlock, can start job

            // Do the job without holding any locks
            job();

            // Count work done
            ++_jobsDone;
        }
    }

public:

    static void start_and_display(const JobQueue_t& jobs)
    {
        static const size_t _BAR_WIDTH = 50;

        size_t total    = jobs.size();
        size_t current  = 0;
        size_t position = 0;
        float progress  = 0;

        {   // empty scope start -> to control the lifecycle of the pool
            ThreadPool<NThread> pool(jobs);

            while (current < total)
            {
                current = pool.jobs_done();
                
                // Draw Progress
                progress = static_cast<float>(current) / static_cast<float>(total);
                position = static_cast<size_t>(_BAR_WIDTH * progress);

                std::cout << "[";
                for (size_t i = 0; i < _BAR_WIDTH; ++i)
                {
                    if (i < position)
                        std::cout << "=";
                    else if (i == position)
                        std::cout << ">";
                    else
                        std::cout << " ";
                }
                std::cout << "] " << static_cast<size_t>(progress * 100.0f) << " %\r";
                std::cout.flush();

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }   // empty scope end -> ThreadPool obj is destroyed, but waits for the threads to finish and join

        // End Display
        std::cout << '\n';
        std::cout.flush();
    }
};  // END ThreadPool