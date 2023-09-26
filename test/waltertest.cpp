#include <cstdint>
#include <gtest/gtest.h>
#include <cmath>
#include <iostream>
#include <string>
#include <tuple>


static constexpr uint32_t MAX_RTX_BANDWIDTH_PERCENT = 25;
static constexpr float MAX_RTX_LOSS_RATIO = (float)MAX_RTX_BANDWIDTH_PERCENT/100;


using namespace std;

static uint32_t calculateMediaBitrate(uint32_t totalBitrate, float lossRatio) {
    float rtxLossRatio = lossRatio * std::pow(1 + lossRatio, 3);
    rtxLossRatio = std::min(rtxLossRatio, MAX_RTX_LOSS_RATIO);
    cout << "rtxLossRatio=" << rtxLossRatio << endl;
    return uint32_t(totalBitrate / (1 + rtxLossRatio));
}


