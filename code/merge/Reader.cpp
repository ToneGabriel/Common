#include "Reader.h"

typename Reader::iterator Reader::begin() { return _container.begin(); }

typename Reader::iterator Reader::end() { return _container.end(); }

typename Reader::const_iterator Reader::begin() const { return _container.begin(); }

typename Reader::const_iterator Reader::end() const { return _container.end(); }

void Reader::read(const std::string& filename)
{
    reset();

    std::ifstream file(filename);

    if (file.is_open())
    {
        std::string line;
        size_t pos;

        while (std::getline(file, line))
        {
            pos = line.find(' '); // Find the _position of space separating path and number

            if (pos != std::string::npos && pos < line.length() - 1)
            {
                int number          = std::stoi(line.substr(0, pos)); // Extract number
                std::string path    = line.substr(pos + 1); // Extract path

                _container.push_back({number, path});
            }
            else
            {
                _destination = line;
            }
        }

        file.close();
    }
    else
        throw std::runtime_error("Unable to open file: " + filename);
}

const std::string& Reader::dest() const
{
    return _destination;
}

void Reader::reset()
{
    _container.clear();
    _destination.clear();
}