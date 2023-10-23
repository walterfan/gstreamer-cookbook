#pragma once
#include <string>
#include <chrono>

constexpr auto ISO_8601_TIME_FMT = "%Y-%m-%dT%H:%M:%SZ";
constexpr auto DEFAUT_TIME_FMT = "%Y%m%d%H%M%S";

//format a timestamp as an ISO 8601 date-time string
std::string get_current_time(const std::string& format = ISO_8601_TIME_FMT);

std::string get_time_str(
    const std::chrono::system_clock::time_point& timePoint,
    const std::string& strPattern);