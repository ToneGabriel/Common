#include "ProgressBar.h"

void ProgressBar::run()
{
    _totalIterations = _callables.size();

    for (_currentIteration = 0; _currentIteration < _totalIterations; ++_currentIteration)
    {
        _callables[_currentIteration]();
        _draw_progress();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void ProgressBar::add_task(std::function<void()>&& func)
{
    _callables.push_back(std::move(func));
}

void ProgressBar::clear()
{
    _callables.clear();
}

void ProgressBar::_draw_progress()
{
    _progress = static_cast<float>(_currentIteration + 1) / _totalIterations;
    _position = barWidth * _progress;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < _position) std::cout << "=";
        else if (i == _position) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(_progress * 100.0f) << " %\r";
    std::cout.flush();
}