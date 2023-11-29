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

std::string str_tolower(std::string s);

bool is_number(const std::string& s);

int find_closed_smaller_value(std::vector<std::string>& numbers, const std::string& target);

int find_closed_bigger_value(std::vector<std::string>& numbers, const std::string& target);

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
