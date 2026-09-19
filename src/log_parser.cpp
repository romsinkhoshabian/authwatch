#include "log_parser.hpp"

#include <fstream>
#include <regex>

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

std::optional<LogEvent> parse_line(const std::string& line) {
    static const std::regex failed_re(
        R"(^(\w{3}\s+\d+ \d{2}:\d{2}:\d{2}) \S+ sshd\[\d+\]: Failed password for (invalid user )?(\S+) from (\S+) port (\d+))");

    std::smatch m;
    if (std::regex_search(line, m, failed_re)) {
        LogEvent ev;
        ev.type = EventType::FailedPassword;
        ev.timestamp = m[1].str();
        ev.user = m[3].str();
        ev.ip = m[4].str();
        ev.port = std::stoi(m[5].str());
        return ev;
    }

    return std::nullopt;
}
