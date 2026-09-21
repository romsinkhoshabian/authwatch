#include "log_parser.hpp"

#include <fstream>
#include <iomanip>
#include <regex>
#include <sstream>

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
    static const std::regex accepted_re(
        R"(^(\w{3}\s+\d+ \d{2}:\d{2}:\d{2}) \S+ sshd\[\d+\]: Accepted password for (\S+) from (\S+) port (\d+))");
    static const std::regex invalid_re(
        R"(^(\w{3}\s+\d+ \d{2}:\d{2}:\d{2}) \S+ sshd\[\d+\]: Invalid user (\S+) from (\S+) port (\d+))");

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

    if (std::regex_search(line, m, accepted_re)) {
        LogEvent ev;
        ev.type = EventType::AcceptedPassword;
        ev.timestamp = m[1].str();
        ev.user = m[2].str();
        ev.ip = m[3].str();
        ev.port = std::stoi(m[4].str());
        return ev;
    }

    if (std::regex_search(line, m, invalid_re)) {
        LogEvent ev;
        ev.type = EventType::InvalidUser;
        ev.timestamp = m[1].str();
        ev.user = m[2].str();
        ev.ip = m[3].str();
        ev.port = std::stoi(m[4].str());
        return ev;
    }

    return std::nullopt;
}

std::optional<std::time_t> parse_timestamp(const std::string& text, int year) {
    std::tm tm{};
    std::istringstream in(text);
    in >> std::get_time(&tm, "%b %d %H:%M:%S");
    if (in.fail()) {
        return std::nullopt;
    }
    tm.tm_year = year - 1900;
    return timegm(&tm);
}
