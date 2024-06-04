#pragma once
#include "ThreadPool.h"


class ThreadPoolDisplay
{
private:
    std::queue<std::function<void(void)>>   _temporaryJobs;
    size_t                                  _totalIterations;
    size_t                                  _currentIteration;
    size_t                                  _position;
    float                                   _progress;

    static constexpr size_t _BAR_WIDTH = 50;

public:

    ThreadPoolDisplay() = default;
    ~ThreadPoolDisplay()
    {
        _currentIteration = _totalIterations;
        _draw_progress();   // what remains
    }

    void add_job(std::function<void(void)>&& func)
    {
        _temporaryJobs.emplace(std::move(func));
    }

    void start()
    {
        ThreadPool pool(4);

        _totalIterations = _temporaryJobs.size();
        while (!_temporaryJobs.empty())
        {
            pool.do_job(std::move(_temporaryJobs.front()));
            _temporaryJobs.pop();
        }

        while (!pool._jobs.empty())
        {
            _currentIteration = _totalIterations - pool._jobs.size();
            _draw_progress();
        }

        // pool is destroyed, but waits for the threads to finish and join
    }

private:

    void _draw_progress()
    {
        _progress = static_cast<float>(_currentIteration) / _totalIterations;
        _position = _BAR_WIDTH * _progress;

        std::cout << "[";
        for (int i = 0; i < _BAR_WIDTH; ++i)
        {
            if (i < _position) std::cout << "=";
            else if (i == _position) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(_progress * 100.0f) << " %\r";
        std::cout.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
};