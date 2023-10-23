#pragma once
#include <string>
#include <cstdint>
#include <sstream>

bool endswith(const std::string& filename, const std::string& suffix);

bool startswith(const std::string& filename, const std::string& prefix);

template<class T>
T str_to_num(const std::string timestr) {
    T value;
    std::istringstream iss(timestr);
    iss >> value;
    return value;
}