#pragma once

#include <gst/gst.h>
#include <glib.h>
#include <string>
#include <map>

class PipelineController {
public:
    PipelineController();
    virtual ~PipelineController();
    int init(int argc, char *argv[]);
    int clean();

    int start();
    int stop();

    int pause();
    int resume();

private:
    bool create_elements();
    bool link_elements();
    void unlink_elements();

    GstElement* create_element(const std::string& factory, const std::string& name);
    int setup_elements();

    std::string m_video_source;
    std::string m_video_target;

    GstElement* m_source_element;
    GstElement* m_target_element;
    GstElement* m_tee_element;
    GstElement* m_enc_element;

    std::map<std::string, GstElement*> m_elements;
    
    GMainLoop* m_loop;
    GstElement* m_pipeline;
    GstBus* m_bus;
    gulong m_probe_id;
};
