#pragma once
#include <string>
#include <cstdint>
#include <sstream>

bool endswith(const std::string& filename, const std::string& suffix);

bool startswith(const std::string& filename, const std::string& prefix);

std::string trim_prefix_suffix(std::string& filename, 
    const std::string& prefix, 
    const std::string& suffix);

std::string substr_no_prefix(std::string& filename, 
    const std::string& prefix, uint32_t len);


template<class T>
T str_to_num(const std::string timestr) {
    T value;
    std::istringstream iss(timestr);
    iss >> value;
    return value;
}

int find_closed_smaller_value(std::vector<std::string>& numbers, const std::string& target);

int find_closed_bigger_value(std::vector<std::string>& numbers, const std::string& target);
