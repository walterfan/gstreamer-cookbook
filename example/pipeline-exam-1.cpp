#include "pipeline-exam-1.h"

int PipelineController::init(int argc, char *argv[]) {
    return 0;
}

int PipelineController::clean() {
    //don't forget gst_object_unref (GST_OBJECT (element)) if not use it
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
        g_print ("Failed to create element %s of type %s\n", name.c_str(), factory.c_str());
        return nullptr;
    }
    m_elements.emplace(std::make_pair(name, e));
    return element;
}

int main(int argc, char const *argv[]) {
    return 0;

}