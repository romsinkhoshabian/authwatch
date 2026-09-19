#include "log_parser.hpp"

#include <fstream>

int count_lines(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return -1;
    }

    int count = 0;
    std::string line;
    while (std::getline(file, line)) {
        ++count;
    }
    return count;
}
