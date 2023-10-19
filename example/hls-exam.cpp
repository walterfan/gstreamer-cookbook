#include <chrono>
#include <thread>
#include "pipeline_controller.h"

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

