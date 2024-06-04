#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

class LineReader
{
public:
    using container         = std::vector<std::string>;
    using iterator          = typename container::iterator;
    using const_iterator    = typename container::const_iterator;

private:
    container _lines;

public:
    LineReader() = default;

    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    void read(const std::string& filename);

    void reset();
};
