#include <chrono>
#include <thread>
#include "gst_util.h"
#include "pipeline_builder.h"

int main(int argc, char *argv[]) {
    auto verifier = std::make_unique<PipelineBuilder>();
    int ret = verifier->init(argc, argv);
    if (ret < 0) {
        DEBUG_TRACE("initialize failed:" << ret 
            << "\ne.g.  ./bin/gst-pipeline-verify -f ./example/etc/pipeline.yaml -p pipeline_test_rtmp" );
        return ret;
    }
    verifier->build();
    verifier->start();
    verifier->stop();
    verifier->clean();
}

