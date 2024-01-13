#pragma once
#include <string>
#include <chrono>

constexpr auto ISO_8601_TIME_FMT = "%Y-%m-%dT%H:%M:%SZ";
constexpr auto DEFAUT_TIME_FMT = "%Y%m%d%H%M%S";
constexpr auto SHORT_TIME_FMT = "%y%m%d%H%M%S";

using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

//format a timestamp as an ISO 8601 date-time string
std::string get_current_time(const std::string& format = ISO_8601_TIME_FMT);

std::string get_time_str(
    const std::chrono::system_clock::time_point& timePoint,
    const std::string& timeFormat = ISO_8601_TIME_FMT);

inline std::string get_time_t_str(
    const std::time_t timestamp,
    const std::string& timeFormat = ISO_8601_TIME_FMT) {
    return get_time_str(std::chrono::system_clock::from_time_t(timestamp), timeFormat);
};

std::string get_format_time(
    const std::chrono::system_clock::time_point& timePoint,
    const std::string& strPattern,
    const std::string& timeFormat = DEFAUT_TIME_FMT);

TimePoint get_timepoint(const std::string& strTime, 
    const std::string& timeFormat = DEFAUT_TIME_FMT);