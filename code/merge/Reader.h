#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <unordered_map>

class Reader
{
public:
    using container         = std::vector<std::pair<int, std::string>>;
    using iterator          = typename container::iterator;
    using const_iterator    = typename container::const_iterator;

private:
    container _container;
    std::string _destination;

public:
    Reader() = default;

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    void read(const std::string& filename);

    const std::string& dest() const;

    void reset();
};
