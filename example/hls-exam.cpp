#include <gst/gst.h>
#include <glib.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <fmt/core.h>
#include "pipeline_controller.h"

/*
gst-launch-1.0 avfvideosrc device-index=1 ! \
video/x-raw,width=1920,height=1080,format=UYVY,framerate=30/1 ! autovideosink
*/
int main(int argc, char *argv[]) {
    auto controller = std::make_unique<PipelineController>();
    controller->init(argc, argv);
    controller->start();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    controller->pause();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    controller->resume();
    controller->stop();
    controller->clean();
}

