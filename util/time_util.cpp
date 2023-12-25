#include "time_util.h"

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <iostream>


//UTC time
std::string get_current_time(const std::string& format) {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::gmtime(&currentTime);

    std::ostringstream ss;
    ss << std::put_time(timeInfo, format.c_str());
    return ss.str();
}


std::string get_time_str(
    const std::chrono::system_clock::time_point& timePoint,
    const std::string& timeFormat) {
    auto in_time_t = std::chrono::system_clock::to_time_t(timePoint);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), timeFormat.c_str());
    return ss.str();    
}

std::string get_format_time(
    const std::chrono::system_clock::time_point& timePoint,
    const std::string& strPattern,
    const std::string& timeFormat)
{
    auto in_time_t = std::chrono::system_clock::to_time_t(timePoint);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), timeFormat.c_str());
    char buffer[1024] = {0};
    snprintf(buffer, 1024, strPattern.c_str(), ss.str().c_str());
    return buffer;

}

TimePoint get_timepoint(const std::string& strTime, const std::string& strPattern) {
    std::istringstream ss(strTime);
    std::tm timeInfo = {};
    ss >> std::get_time(&timeInfo, strPattern.c_str());

    return std::chrono::system_clock::from_time_t(std::mktime(&timeInfo));
}
