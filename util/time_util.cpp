#include "time_util.h"

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <fmt/core.h>


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
    const std::string& strPattern)
{
    auto in_time_t = std::chrono::system_clock::to_time_t(timePoint);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), DEFAUT_TIME_FMT);
    return fmt::format(fmt::runtime(strPattern), ss.str());
}