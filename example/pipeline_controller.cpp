#include <iostream>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <chrono>
#include <thread>
#include <gst/gst.h>
#include <glib.h>
#include <filesystem>
#include "time_util.h"
#include "gst_util.h"
#include "pipeline_controller.h"

namespace fs = std::filesystem;

static const GstPadProbeType pad_probe_type = GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM;

static uint32_t deleted_fragments = 0;

static gboolean delete_fragment_callback(GstElement *element, const gchar *uri, gpointer user_data) {
    // Your custom logic for handling fragment deletion here.
    // In this example, we will simply print a message.
    DEBUG_TRACE(++deleted_fragments << ". Deleted fragment: " << uri);
    return TRUE;
}

static GstPadProbeReturn block_downstream_probe(GstPad *pad, GstPadProbeInfo *info, gpointer user_data) {
    // Block the downstream data flow by returning FALSE in the probe function.
    DEBUG_TRACE("blocking stream...");
    return GST_PAD_PROBE_OK;
}

PipelineController::PipelineController()
: m_loop(nullptr)
, m_pipeline(nullptr)
, m_bus(nullptr)
, m_probe_id(0) {
    DEBUG_TRACE("PipelineController construct");
}

PipelineController::~PipelineController()
{
    DEBUG_TRACE("PipelineController destruct");
}

int PipelineController::init(int argc, char *argv[]) {
    gst_init(&argc, &argv);
    DEBUG_TRACE("PipelineController init");
    const std::vector<std::string_view> args(argv, argv + argc);
    const std::string_view video_source_plugin = get_option(args, "-s");
    const std::string_view video_target_plugin = get_option(args, "-t");

    m_video_source = "videotestsrc";
    m_video_target = "hlssink2";

    if (!video_source_plugin.empty()) {
        m_video_source = video_source_plugin;
    }

    if (!video_target_plugin.empty()) {
        m_video_target = video_target_plugin;
    }
    create_elements();
    setup_elements();
    link_elements();
    return 0;
}
int PipelineController::clean() {
    DEBUG_TRACE("PipelineController clean");
   
    //gst_object_unref to free pipeline resources including all added GstElement objects
    gst_object_unref(m_pipeline);
    gst_object_unref(m_bus);

    return 0;
}

int PipelineController::start() {
    DEBUG_TRACE("PipelineController start");
    //check_pads(m_target_element);
    
    std::string dot_file = "video_pipeline";
    //set environment variable, such as export GST_DEBUG_DUMP_DOT_DIR=/tmp
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN_CAST(m_pipeline), GST_DEBUG_GRAPH_SHOW_VERBOSE, dot_file.c_str());

    DEBUG_TRACE("start playing...");
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
    return 0;
}
int PipelineController::stop() {
    DEBUG_TRACE("stop playing...");
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    return 0;
}

int PipelineController::pause() {
    DEBUG_TRACE("pause playing...");
    m_probe_id = 0;
    // Get the source pad of hlssink
    GstPad *hlssink_pad = gst_element_get_static_pad(m_target_element, VIDEO_PAD_NAME);
    if(hlssink_pad) {
        DEBUG_TRACE("to block stream");
        m_probe_id = gst_pad_add_probe(hlssink_pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, block_downstream_probe, NULL, NULL);
    }
    
    return m_probe_id;
}

int PipelineController::resume() {
    DEBUG_TRACE("resume playing...");
    if (!m_probe_id) {
        DEBUG_TRACE("have not paused");
        return -1;
    }
    GstPad *hlssink_pad = gst_element_get_static_pad(m_target_element, VIDEO_PAD_NAME);
    if(hlssink_pad) {
        DEBUG_TRACE("to unblock stream");
        gst_pad_remove_probe(hlssink_pad, m_probe_id);
        m_probe_id = 0;
    }
    
    return 0;
    
}


bool PipelineController::create_elements() {
    DEBUG_TRACE("PipelineController create_elements");
    m_pipeline = gst_pipeline_new("video-pipeline");
    m_bus = gst_element_get_bus(m_pipeline);
    m_source_element = create_element(m_video_source, "video-source");
    m_tee_element = create_element("tee", "video-tee");
    m_enc_element = create_element("x264enc", "video-encoder");
    m_target_element = create_element(m_video_target, "video-target");

    if(m_source_element && m_tee_element && m_enc_element && m_target_element) {
        return true;
    }
    return false;
}

int PipelineController::setup_elements() {
    DEBUG_TRACE("PipelineController setup_elements");
    if (m_video_source == "videotestsrc") {
        g_object_set(m_source_element, "pattern", 0, NULL); // Set the test pattern
    } else if(m_video_source == "avfvideosrc") {
        g_object_set(m_source_element, "device-index", 0, NULL); // Set the test pattern
        
        GstCaps* caps = gst_caps_new_simple("video/x-raw",       
            "width", G_TYPE_INT, 1920,                               
            "height", G_TYPE_INT, 1080,                              
            "framerate", GST_TYPE_FRACTION, 30, 1, NULL);
             
        g_object_set(G_OBJECT(m_source_element), "caps", caps, nullptr);
        gst_caps_unref(caps); 
        
    } else {
        DEBUG_TRACE("unknown source element");
    }

    auto now = std::chrono::system_clock::now();
    fs::create_directories("/tmp/record/");
    std::string playlist_filename = get_format_time(now, "/tmp/record/playlist_{}.m3u8");
    std::string record_filename = get_format_time(now, "/tmp/record/record_{}_%05d.ts");

    DEBUG_TRACE("playlist filename: " << playlist_filename 
        << ", record_filename=" << record_filename);

    g_object_set(m_target_element, "location", record_filename.c_str(), NULL);
    g_object_set(m_target_element, "playlist-location", playlist_filename.c_str(), NULL);
    //g_object_set(m_target_element, "playlist-root", "/tmp", NULL);
    g_object_set(m_target_element, "playlist-length", 20, NULL);
    g_object_set(m_target_element, "max-files", 20, NULL);
    g_object_set(m_target_element, "target-duration", 10, NULL);

    g_signal_connect(G_OBJECT(m_target_element), "delete-fragment", G_CALLBACK(delete_fragment_callback), NULL);
    return 0;
}

bool PipelineController::link_elements() {
    DEBUG_TRACE("add elements");
    gst_bin_add_many(GST_BIN(m_pipeline), m_source_element, m_tee_element, m_enc_element, m_target_element, NULL);
    DEBUG_TRACE("link_elements");
    gst_element_link_many(m_source_element, m_tee_element, m_enc_element, m_target_element, NULL);
    
    return true;
}
void PipelineController::unlink_elements() {
    DEBUG_TRACE("unlink_elements");
    gst_element_unlink_many(m_source_element, m_tee_element, m_enc_element, m_target_element, NULL);

    DEBUG_TRACE("remove elements");
    gst_bin_remove_many(GST_BIN(m_pipeline), m_source_element, m_tee_element, m_enc_element, m_target_element, NULL);
  
}

GstElement* PipelineController::create_element(
    const std::string& factory, 
    const std::string& name) {
    DEBUG_TRACE("create_element:" << factory << ", name=" << name);
    GstElement* e = gst_element_factory_make(factory.c_str(), name.c_str());
    m_elements.emplace(std::make_pair(name, e));
    return e;
}
