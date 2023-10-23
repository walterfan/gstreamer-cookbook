#include <cstdint>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "string_util.h"
#include "time_util.h"
#include "hls_util.h"

static constexpr uint32_t MAX_RTX_BANDWIDTH_PERCENT = 25;
static constexpr float MAX_RTX_LOSS_RATIO = (float)MAX_RTX_BANDWIDTH_PERCENT / 100;

static uint32_t calculateMediaBitrate(uint32_t totalBitrate, float lossRatio)
{
    float rtxLossRatio = lossRatio * std::pow(1 + lossRatio, 3);
    rtxLossRatio = std::min(rtxLossRatio, MAX_RTX_LOSS_RATIO);
    std::cout << "rtxLossRatio=" << rtxLossRatio << std::endl;
    return uint32_t(totalBitrate / (1 + rtxLossRatio));
}

int main(int argc, char *argv[])
{
    auto now = std::chrono::system_clock::now();

    std::string path = "/tmp";
    std::vector<std::string> fileNames;
    int ret = get_playlist_files(path, "playlist_", ".m3u8", fileNames);

    int pos = find_closed_small_value(fileNames, "20231019225220");
    if (pos >= 0 && pos < fileNames.size()) {
        std::cout << pos <<". filename: playlist_"<< fileNames[pos] << ".m3u8" << std::endl;
    }

    std::vector<std::string> segments{
        "record_20231019160652_08786.ts", 
        "record_20231019160652_08787.ts"};

    HlsPlaylist playlist;
    playlist.m_target_duration = 10;
    playlist.m_start_tag.m_time_offset = 0;
    playlist.m_start_tag.m_precise = "YES";
    playlist.m_segments.push_back({9.5, "record_20231019160652_08786.ts" });
    playlist.m_segments.push_back({8.7, "record_20231019160652_08787.ts" });    

    
    std::string filename = get_time_str(now, "short_video_{}.m3u8");
    ret = write_m3u8_file(filename, playlist);
    std::cout << "write m3u8 file " << filename << ", ret=" << ret << std::endl;
    return 0;
}