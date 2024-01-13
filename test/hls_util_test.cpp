#include "gtest/gtest.h"

#include "time_util.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include "string_util.h"
#define private public
#include "hls_util.h"
using namespace std;
using namespace testing;

class HlsUtilTest: public testing::Test
{
public:

    virtual void SetUp() {
        m_numbers = {
            "20231019222811",
            "20231019222831",
            "20231019223358",
            "20231019223501",
            "20231019223753",
            "20231019224240",
            "20231019224431",
            "20231019225017",
            "20231019225323",
            "20231019225422"
        };
    }

protected:
    std::vector<std::string> m_numbers;
};

TEST_F(HlsUtilTest, find_closed_smaller_value_first)
{
    
    std::string bigger_value = "20231019222811";
    int pos = find_closed_smaller_value(m_numbers, bigger_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 0);

    bigger_value = "20231019222812";
    pos = find_closed_smaller_value(m_numbers, bigger_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 0);
}

TEST_F(HlsUtilTest, find_closed_smaller_value_equal)
{
    
    std::string bigger_value = "20231019223753";
    int pos = find_closed_smaller_value(m_numbers, bigger_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 4);

    bigger_value = "20231019225323";
    pos = find_closed_smaller_value(m_numbers, bigger_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 8);
}

TEST_F(HlsUtilTest, find_closed_smaller_value_greater)
{
    
    std::string bigger_value = "20231019223853";
    int pos = find_closed_smaller_value(m_numbers, bigger_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 4);

    bigger_value = "20231019224340";
    pos = find_closed_smaller_value(m_numbers, bigger_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 5);
}

TEST_F(HlsUtilTest, find_closed_bigger_value)
{
    
    std::string smaller_value = "20231019225000";
    int pos = find_closed_bigger_value(m_numbers, smaller_value);
    std::cout << "pos=" << pos << std::endl;
    ASSERT_EQ(pos, 7);
    ASSERT_EQ(m_numbers[7], "20231019225017");
}

TEST_F(HlsUtilTest, write_m3u8_file) {
    auto now = std::chrono::system_clock::now();
    std::string filename = get_format_time(now, "snapshot_%s.m3u8");

    HlsPlaylist playlist(filename);
    playlist.m_target_duration = 10;
    playlist.m_start_tag.m_time_offset = 0;
    playlist.m_start_tag.m_precise = "YES";
    playlist.m_segments.push_back({9.5, "record_20231019160652_08786.ts" });
    playlist.m_segments.push_back({8.7, "record_20231019160652_08787.ts" });    
    playlist.m_segments.push_back({10, "record_20231019160652_08788.ts" }); 
    playlist.m_segments.push_back({10, "record_20231019160652_08789.ts" }); 
    playlist.m_segments.push_back({10, "record_20231019160652_08790.ts" }); 
    
    std::cout << "# begin wrote m3u8 file: "<< std::endl;
    int ret = playlist.write_m3u8_file();
    std::cout << "-- end wrote m3u8 file: " << playlist << ", ret=" << ret << std::endl;
    ASSERT_EQ(ret, 0);

    std::cout << "# begin read m3u8 file: "<< std::endl;
    HlsPlaylist playlist2(filename);
    ret = playlist2.read_m3u8_file();
    std::cout << "-- end read m3u8 file: " << playlist2 << ", ret=" << ret << std::endl;
    ASSERT_EQ(ret, 5);
}


TEST_F(HlsUtilTest, update_uri) {
    std::string ts_name = "record_20231023170112_00043.ts";
    HlsSegment hlsSegment(10, ts_name);
    uint32_t seq = hlsSegment.get_sequence();
    ASSERT_EQ(seq, 43);


    int ret = hlsSegment.update_uri(++seq);
    std::cout << "uri=" << hlsSegment.m_uri << std::endl;

    ret = hlsSegment.update_uri(3);
    std::cout << "uri=" << hlsSegment.m_uri  << std::endl;
}


TEST_F(HlsUtilTest, read_m3u8_file) {
    
    std::string m3u8_file0 = "../test/playlist_20240111141043.m3u8";
    HlsPlaylist playlist0(m3u8_file0);
    playlist0.read_m3u8_file();
    std::cout << playlist0 << std::endl;
    
    std::string m3u8_file1 = "../test/playlist_20240111145046.m3u8";
    HlsPlaylist playlist1(m3u8_file1);
    playlist1.read_m3u8_file();
    std::cout << playlist1 << std::endl;
}

