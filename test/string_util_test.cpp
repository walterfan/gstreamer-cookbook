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


TEST(StringUtilTest, substr_no_prefix)
{
    std::string ts_filename = "/tmp/record_20231019233046_00029.ts";
    std::string m3u8_filename = "/tmp/playlist_20231019233046.m3u8";

    std::string ts_time = substr_no_prefix(ts_filename, "/tmp/record_", 14);
    std::string m3u8_time = substr_no_prefix(m3u8_filename, "/tmp/playlist_", 14);
    
    std::cout << "ts_time: " << ts_time << std::endl;
    std::cout << "m3u8_time: " << m3u8_time << std::endl;

    std::chrono::time_point ts_timestamp = get_timepoint(ts_time);
    std::chrono::time_point m3u8_timestamp = get_timepoint(m3u8_time);

    std::cout << "ts_timestamp: " << std::chrono::system_clock::to_time_t(ts_timestamp) << std::endl;
    std::cout << "m3u8_timestamp: " << std::chrono::system_clock::to_time_t(m3u8_timestamp) << std::endl;

    ASSERT_EQ(ts_time, get_time_str(ts_timestamp, DEFAUT_TIME_FMT));
    ASSERT_EQ(m3u8_time, get_time_str(m3u8_timestamp, DEFAUT_TIME_FMT));
}
