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
private:
    static_assert(NThread < MACHINE_NUM_PROCESSORS, "Number of threads exceeds the system capability!");

    template<class Container, class = void>
    struct _HasValidMethods : std::false_type {};

    template<class Container>
    struct _HasValidMethods<Container, std::void_t< decltype(std::declval<Container>().clear()),
                                                    decltype(std::declval<Container>().size()),
                                                    decltype(std::declval<Container>().begin()),
                                                    decltype(std::declval<Container>().end())>> : std::true_type {};

    template<class Container>
    using _EnableThreadPoolFromContainer_t =
                std::enable_if_t<std::conjunction_v<
                                        std::is_same<typename Container::value_type, std::function<void(void)>>,
                                        std::is_convertible<typename std::iterator_traits<typename Container::iterator>::iterator_category, std::forward_iterator_tag>,
                                        _HasValidMethods<Container>>,
                bool>;

private:
    std::queue<std::function<void(void)>>   _jobs;
    std::thread                             _workerThreads[NThread];
    std::mutex                              _poolMtx;
    std::condition_variable                 _poolCV;
    std::atomic_size_t                      _jobsDone;
    bool                                    _shutdown;

public:
    // Constructors & Operators

    ThreadPool()
    {
        _open_pool();
    }

    ~ThreadPool()
    {
        _close_pool();
    }

    ThreadPool(const ThreadPool& other)             = delete;
    ThreadPool& operator=(const ThreadPool& other)  = delete;

public:
    // Main functions

    void do_job(std::function<void()>&& newJob)
    {
        // Move a job on the queue and unblock a thread
        std::unique_lock<std::mutex> lock(_poolMtx);

        _jobs.emplace(std::move(newJob));
        _poolCV.notify_one();
    }

    template<class JobContainer, _EnableThreadPoolFromContainer_t<JobContainer> = true>
    void do_more_jobs(JobContainer&& newJobs)
    {
        // Move multiple jobs on the queue and unblock necessary threads

        std::unique_lock<std::mutex> lock(_poolMtx);

        // Save added size
        const size_t addedSize = newJobs.size();

        // Move jobs from container and clear it
        for (auto&& val : newJobs)
            _jobs.emplace(std::move(val));
        newJobs.clear();

        // Notify waiting threads
        for (size_t i = 0; i < addedSize; ++i)
            _poolCV.notify_one();
    }

    size_t jobs_done() const
    {
        return _jobsDone;
    }

private:
    // Helpers

    void _open_pool()
    {
        _jobsDone       = 0;
        _shutdown       = false;

        // Create the specified number of threads
        for (auto& t : _workerThreads)
            t = std::thread(std::bind(&ThreadPool<NThread>::_worker_thread, this));
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

    void _worker_thread()
    {
        std::function<void()> job;

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
            try
            {
                job();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Job threw an exception: " << e.what() << '\n';
            }
            catch (...)
            {
                std::cerr << "Job threw an unknown exception." << '\n';
            }

            // Count work done
            ++_jobsDone;
        }
    }

public:

    template<class JobContainer, _EnableThreadPoolFromContainer_t<JobContainer> = true>
    static void start_and_display(JobContainer&& newJobs)
    {
        static const size_t _BAR_WIDTH = 50;

        size_t total    = newJobs.size();
        size_t current  = 0;
        size_t position = 0;
        float progress  = 0;

        {   // empty scope start -> to control the lifecycle of the pool
            ThreadPool<NThread> pool;
            pool.do_more_jobs(std::move(newJobs));

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