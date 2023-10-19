#include "pipeline-exam-1.h"

int PipelineController::init(int argc, char *argv[]) {
    return 0;
}

int PipelineController::clean() {
    return 0;
}

int PipelineController::start() {
    return 0;
}

int PipelineController::stop() {
    return 0;
}

GstElement* PipelineController::create_element(const std::string& factory, 
    const std::string& name) {
    GstElement* element = gst_element_factory_make (factory.c_str(), name.c_str());
    if (!element) {
        g_print ("Failed to create element of type 'fakesrc'\n");
        return nullptr;
    }
    //don't forget gst_object_unref (GST_OBJECT (element)) if not use it
    return element;
}

int main(int argc, char const *argv[]) {
    return 0;

}