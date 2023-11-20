#pragma once

#include <gst/gst.h>
#include <glib.h>
#include <memory>
#include <thread>
#include <vector>
#include <map>

class PipelineController {
public:
    int init(int argc, char *argv[]);
    int clean();

    int start();
    int stop();

private:

    GstElement* create_element(const std::string& factory, 
    const std::string& name);

    std::map<std::string, GstElement*> m_elements;
    GstBus* m_bus;
    GMainLoop* m_loop;
    GstElement* m_pipeline;
};
