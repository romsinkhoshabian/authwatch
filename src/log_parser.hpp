#pragma once

#include <ctime>
#include <optional>
#include <string>

enum class EventType {
    FailedPassword,
    InvalidUser,
    AcceptedPassword
};

struct LogEvent {
    EventType   type;
    std::string timestamp;   // raw text like "Sep 19 10:15:32"
    std::string user;
    std::string ip;
    int         port;
};

// Returns the number of lines in the file at `path`,
// or -1 if the file cannot be opened.
int count_lines(const std::string& path);

// Parses one sshd log line. Returns std::nullopt if the line is not
// a recognized SSH authentication event.
std::optional<LogEvent> parse_line(const std::string& line);

// Converts a syslog timestamp such as "Sep 19 10:20:01" into seconds since
// the Unix epoch (UTC). Syslog timestamps carry no year, so the caller
// supplies it. Returns std::nullopt if the text is not a valid timestamp.
std::optional<std::time_t> parse_timestamp(const std::string& text, int year);
