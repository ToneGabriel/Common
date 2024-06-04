#include "LineReader.h"

typename LineReader::iterator LineReader::begin() { return _lines.begin(); }

typename LineReader::iterator LineReader::end() { return _lines.end(); }

typename LineReader::const_iterator LineReader::begin() const { return _lines.begin(); }

typename LineReader::const_iterator LineReader::end() const { return _lines.end(); }

void LineReader::read(const std::string& filename)
{
    reset();

    std::ifstream file(filename);

    if (file.is_open())
    {
        std::string line;

        while (std::getline(file, line))
        {
            _lines.push_back(std::move(line));
            line.clear();
        }

        file.close();
    }
    else
        throw std::runtime_error("Unable to open file: " + filename);
}

void LineReader::reset()
{
    _lines.clear();
}