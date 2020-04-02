#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#include "hexdump.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
    {
        fmt::print("Usage: {} <file>", av[0]);
        return 1;
    }
    std::string filename(av[1]);

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line))
    {
        hexdump(line);
    }
}