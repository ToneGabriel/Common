#pragma once
#include <iostream>
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
    ~ThreadPoolDisplay() = default;

    void add_job(std::function<void(void)>&& func);
    void start_and_display();

private:

    void _draw_progress();
};