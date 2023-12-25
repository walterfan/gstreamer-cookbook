#pragma once
#include <string>
#include <cstdint>
#include <sstream>
#include <vector>

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

std::string str_tolower(std::string s);

bool is_number(const std::string& s);

int find_closed_smaller_value(std::vector<std::string>& numbers, const std::string& target);

int find_closed_bigger_value(std::vector<std::string>& numbers, const std::string& target);


inline std::string &trim(std::string &str)
{
    const char *spaces = " \n\r\t";
    str.erase(str.find_last_not_of(spaces) + 1);
    str.erase(0, str.find_first_not_of(spaces));
    return str;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
