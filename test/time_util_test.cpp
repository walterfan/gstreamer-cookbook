#include "gtest/gtest.h"
#include "string_util.h"
#include "time_util.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace testing;


TEST(TimeUtilTest, get_time_t_str)
{

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::gmtime(&currentTime);
    std::string format = "%Y-%m-%dT%H:%M:%SZ";
    std::ostringstream ss;
    ss << std::put_time(timeInfo, format.c_str());
    cout << "current time is " << ss.str() << " --> " << currentTime << endl;
                           //1702014187
    std::time_t begin_time = 1702006134642/1000;
    std::time_t end_time = 1702006139642/1000;

    auto begin_time_point =std::chrono::system_clock::from_time_t(begin_time);

    cout << "begin_time="<< get_time_t_str(begin_time, ISO_8601_TIME_FMT) 
        << ", end_time=" << get_time_t_str(end_time, ISO_8601_TIME_FMT) << endl;
}
