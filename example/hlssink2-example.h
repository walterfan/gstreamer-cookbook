#pragma once

#include <gst/gst.h>
#include <glib.h>


class PipelineController {
public:
    int init(int argc, char *argv[]);
    int clean();

    int start();
    int stop();

private:

    GstElement* create_element(const std::string& factory, 
    const std::string& name);

    std::vector<GstElement*> m_elements;
    GstBus* m_bus;
    GMainLoop* m_loop;
    GstElement* m_pipeline;
};
