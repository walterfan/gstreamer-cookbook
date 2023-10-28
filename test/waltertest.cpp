#include <cstdint>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "string_util.h"
#include "time_util.h"
#include "file_util.h"
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
    std::string path = "/tmp";
    std::vector<std::string> fileNames;
    int ret = get_playlist_files(path, "playlist_", ".m3u8", fileNames);
    for (auto& filename: fileNames) {
        std::cout << filename << std::endl;
    }
    return 0;
}