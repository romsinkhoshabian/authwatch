#pragma once

#include <string>

// Returns the number of lines in the file at `path`,
// or -1 if the file cannot be opened.
int count_lines(const std::string& path);
