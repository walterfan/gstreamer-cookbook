#include "gtest/gtest.h"
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
            char buffer[64] = {0};
            snprintf(buffer, 64, "playlist_%llu.m3u8", num);
            std::string m3u8_file = buffer;
       
            HlsPlaylist playlist((m_test_folder/m3u8_file).string());
            for(int i=0; i<3; ++i) {
                char buffer[128] = {0};
                snprintf(buffer, 128, "record_%llu_0000%d.ts", num, i);
                std::string ts_file = buffer;
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
    for(int i=0; i< playlist_count; ++i) {
        cout << i <<" . playlist:" << *composer.m_src_playlists[i] << endl;
    }
    int playlist_cnt = composer.filter_playlists();
    cout << "filter playlist count=" << playlist_cnt 
    << " from " << get_time_str(m_begin_time) 
    << " to " << get_time_str(m_end_time) << endl;
    ASSERT_EQ(playlist_cnt, 1);
    cout << "selected playlist: " << *(composer.m_selected_playlists[0]) << endl;
    int ret = composer.build_playlist();
    cout << "composed playlist: " << *(composer.m_snapshot_playlist) << endl;
}

TEST_F(PlaylistUtilTest, PlaylistComposerFindOne) {
    m_begin_time = get_timepoint("20231019224805");
    m_end_time   = get_timepoint("20231019224812");

    PlaylistComposer composer(m_test_folder.string(), m_begin_time, m_end_time);
    auto playlist_count = composer.find_playlists();
    cout << "found playlist count=" << playlist_count << endl;
    for(int i=0; i< playlist_count; ++i) {
        cout << i <<" . playlist:" << *composer.m_src_playlists[i] << endl;
    }
    int playlist_cnt = composer.filter_playlists();
    cout << "filter playlist count=" << playlist_cnt 
    << " from " << get_time_str(m_begin_time) 
    << " to " << get_time_str(m_end_time) << endl;
    ASSERT_EQ(playlist_cnt, 1);
    cout << "selected playlist: " << *(composer.m_selected_playlists[0]) << endl;
    int ret = composer.build_playlist();
    cout << "composed playlist: " << *(composer.m_snapshot_playlist) << endl;
}

TEST_F(PlaylistUtilTest, PlaylistComposerFindNothing) {
    m_begin_time = get_timepoint("20231019224845");
    m_end_time   = get_timepoint("20231019224852");

    PlaylistComposer composer(m_test_folder.string(), m_begin_time, m_end_time);
    auto playlist_count = composer.find_playlists();
    cout << "found playlist count=" << playlist_count << endl;
    for(int i=0; i< playlist_count; ++i) {
        cout << i <<" . playlist:" << *composer.m_src_playlists[i] << endl;
    }
    int playlist_cnt = composer.filter_playlists();
    cout << "filter playlist count=" << playlist_cnt 
    << " from " << get_time_str(m_begin_time) 
    << " to " << get_time_str(m_end_time) << endl;
    ASSERT_EQ(playlist_cnt, 1);
    cout << "selected playlist: " << *(composer.m_selected_playlists[0]) << endl;
    int ret = composer.build_playlist();
    ASSERT_EQ(ret, -2);
    cout <<"ret=" << ret<< ", last playlist: " 
        << *(composer.m_src_playlists[playlist_count - 1]) << endl;
    ret = composer.fake_playlist();
    ASSERT_EQ(ret, 0);
    cout << "\nfake_playlist: " << *(composer.m_snapshot_playlist) << endl;
    
}