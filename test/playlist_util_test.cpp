#include "gtest/gtest.h"
#include <fmt/core.h>
#include "time_util.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>
#include "string_util.h"
#define private public
#include "hls_util.h"

using namespace std;
using namespace testing;

namespace fs = std::filesystem;
using sysclock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

class PlaylistUtilTest: public testing::Test
{
public:

    virtual void SetUp() {
        m_numbers = {
            20231019222800,
            20231019223800,
            20231019224800
        };
        
        m_test_folder = "/tmp/record_test"; 
        fs::create_directories(m_test_folder);
        
        for(auto num: m_numbers) {
            std::string m3u8_file = fmt::format(fmt::runtime("playlist_{}.m3u8"), num);
            HlsPlaylist playlist((m_test_folder/m3u8_file).string());
            for(int i=0; i<3; ++i) {
                std::string ts_file = fmt::format(fmt::runtime("record_{}_0000{}.ts"), num, i);
                playlist.append_segment(HlsSegment(10, ts_file));
            }
            playlist.write_m3u8_file();    
        }
        
    }

protected:
    TimePoint m_begin_time;
    TimePoint m_end_time;
    fs::path m_test_folder;
    std::vector<uint64_t> m_numbers;
};


TEST_F(PlaylistUtilTest, PlaylistComposerFind) {
    m_begin_time = get_timepoint("20231019222805");
    m_end_time   = get_timepoint("20231019222817");

    PlaylistComposer composer(m_test_folder.string(), m_begin_time, m_end_time);
    auto playlist_count = composer.find_playlists();
    cout << "found playlist count=" << playlist_count << endl;
    //for(int i=0; i< playlist_count; ++i) {
    //    cout << i <<" . playlist:" << *composer.m_src_playlists[i] << endl;
    //}
    int playlist_cnt = composer.filter_playlists();
    cout << "filter playlist count=" << playlist_cnt 
    << " from " << get_time_str(m_begin_time) 
    << " to " << get_time_str(m_end_time) << endl;
    ASSERT_EQ(playlist_cnt, 1);
    cout << "selected playlist: " << *(composer.m_selected_playlists[0]) << endl;
    int ret = composer.build_playlist();
    cout << "composed playlist: " << *(composer.m_snapshot_playlist) << endl;
}