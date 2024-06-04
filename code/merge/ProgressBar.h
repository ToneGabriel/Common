#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include <functional>


class ProgressBar
{
private:
    std::vector<std::function<void()>> _callables;
    const size_t barWidth = 50;
    size_t _totalIterations;
    size_t _currentIteration;
    size_t _position;
    float _progress;

public:
    ProgressBar() = default;

    void run();

    void add_task(std::function<void()>&& func);

    void clear();

private:

    void _draw_progress();
};