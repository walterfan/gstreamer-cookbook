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
#include <vector>

#include "gst_util.h"

#define DEBUG_TRACE(msg) std::cout << "[" \
    << time(NULL) <<","<< __FILE__ << "," << __LINE__ << "]\t"<< msg << std::endl


static const GstPadProbeType pad_probe_type = GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM;

static uint32_t deleted_fragments = 0;

static std::string get_time_str(
    const std::chrono::system_clock::time_point& timePoint, 
    const std::string& strPrefix, const std::string& strSuffix)
{
    auto in_time_t = std::chrono::system_clock::to_time_t(timePoint);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), TIME_FMT);
    return strPrefix + ss.str() + strSuffix;
}



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
// refer to https://gstreamer.freedesktop.org/documentation/additional/design/probes.html?gi-language=c
gulong start_record(GstElement* hlssink, gulong probe_id) {
    DEBUG_TRACE("to start_record"); 

    GstPad *hlssink_pad = gst_element_get_static_pad(hlssink, VIDEO_PAD_NAME);
    if(hlssink_pad) {
        DEBUG_TRACE("to unblock stream");
        gst_pad_remove_probe(hlssink_pad, probe_id);
    }
    
    return 0;
}

gulong stop_record(GstElement* hlssink) {
    DEBUG_TRACE("to stop_record"); 
    gulong ret = 0;
    // Get the source pad of hlssink
    GstPad *hlssink_pad = gst_element_get_static_pad(hlssink, VIDEO_PAD_NAME);
    if(hlssink_pad) {
        DEBUG_TRACE("to block stream");
        ret = gst_pad_add_probe(hlssink_pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, block_downstream_probe, NULL, NULL);
    }
    
    return ret;
}


void set_element_prop(GstElement* hlssink) {
    auto now = std::chrono::system_clock::now();
    std::string playlist_filename = get_time_str(now, "/tmp/playlist", ".m3u8");
    std::string record_filename = get_time_str(now, "/tmp/record", "_%05d.ts");

    DEBUG_TRACE("playlist filename: " << playlist_filename 
        << ", record_filename=" << record_filename);

    g_object_set(hlssink, "location", record_filename.c_str(), NULL);
    g_object_set(hlssink, "playlist-location", playlist_filename.c_str(), NULL);
    //g_object_set(hlssink, "playlist-root", "/tmp", NULL);
    g_object_set(hlssink, "playlist-length", 20, NULL);
    g_object_set(hlssink, "max-files", 20, NULL);
    g_object_set(hlssink, "target-duration", 10, NULL);
}

int main(int argc, char *argv[]) {
    
    const std::vector<std::string_view> args(argv, argv + argc);
    const std::string_view video_source_plugin = get_option(args, "-s");
    const std::string_view video_target_plugin = get_option(args, "-t");

    std::string video_source = "videotestsrc";
    std::string video_target = "hlssink2";

    if (!video_source_plugin.empty()) {
        video_source = video_source_plugin;
    }

    if (!video_target_plugin.empty()) {
        video_target = video_target_plugin;
    }

    gst_init(&argc, &argv);
    GstElement *pipeline, *avfvideosrc, *videosrc, *tee, *x264enc, *hlssink;

    pipeline = gst_pipeline_new("hls-pipeline");

    videosrc = gst_element_factory_make(video_source.c_str(), "video-source");
    tee = gst_element_factory_make("tee", "video-tee");
    x264enc = gst_element_factory_make("x264enc", "video-encoder");
    hlssink = gst_element_factory_make("hlssink2", "hls-sink");

    if (!pipeline || !videosrc || !x264enc || !hlssink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    if (video_source == "videotestsrc") {
        g_object_set(videosrc, "pattern", 0, NULL); // Set the test pattern
    } else if(video_source == "avfvideosrc") {
        g_object_set(videosrc, "device-index", 0, NULL); // Set the test pattern
        /*
        GstCaps* caps = gst_caps_new_simple("video/x-raw",       
            "width", G_TYPE_INT, 1920,                               
            "height", G_TYPE_INT, 1080,                              
            "framerate", GST_TYPE_FRACTION, 30, 1, NULL);
             
        g_object_set(G_OBJECT(videosrc), "caps", caps, nullptr);
        gst_caps_unref(caps); 
        */
    }

    set_element_prop(hlssink);

    g_signal_connect(G_OBJECT(hlssink), "delete-fragment", G_CALLBACK(delete_fragment_callback), NULL);

    GstBus *bus = gst_element_get_bus(pipeline);

    gst_bin_add_many(GST_BIN(pipeline), videosrc, tee, x264enc, hlssink, NULL);
    gst_element_link_many(videosrc, tee, x264enc, hlssink, NULL);
    
    check_pads(hlssink);

    std::string dot_file = "video_hls_pipeline";
    //set environment variable, such as export GST_DEBUG_DUMP_DOT_DIR=/tmp
    GST_DEBUG_BIN_TO_DOT_FILE(GST_BIN_CAST(pipeline), GST_DEBUG_GRAPH_SHOW_VERBOSE, dot_file.c_str());

    DEBUG_TRACE("start playing for 10s...");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    DEBUG_TRACE("sleep for 10s...\n");
    std::this_thread::sleep_for(std::chrono::seconds(10));

    gulong probe_id = stop_record(hlssink);
    DEBUG_TRACE("stop record for 10s...");
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    set_element_prop(hlssink);
    DEBUG_TRACE("start record for 10s...");
    start_record(hlssink, probe_id);
    std::this_thread::sleep_for(std::chrono::seconds(10));

    DEBUG_TRACE("stop record...");
    probe_id = stop_record(hlssink);
    /*
    DEBUG_TRACE("check message...");
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    if (msg) {
        DEBUG_TRACE("Received message of type " << GST_MESSAGE_TYPE_NAME(msg));
        gst_message_unref(msg);
    }
    */
    DEBUG_TRACE("byebye.");
    gst_element_set_state(pipeline, GST_STATE_NULL);
    
    std::vector<GstElement*> elements{pipeline, videosrc, tee, x264enc, hlssink};
    for(auto el: elements) {
        gchar* name;
        g_object_get (G_OBJECT (el), "name", &name, NULL);
        g_print ("need unlink and remove element: '%s'.\n", name);
        g_free (name);
    }
    gst_object_unref(pipeline);
    gst_object_unref(bus);
    return 0;
}
