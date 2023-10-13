#include <gst/gst.h>
#include <glib.h>
#include <chrono>
#include <thread>
#include <iostream>

#define PAD_NAME "video"
#define DEBUG_TRACE(msg) std::cout << "[" \
    << time(NULL) <<","<< __FILE__ << "," << __LINE__ << "]\t"<< msg << std::endl


static const GstPadProbeType pad_probe_type = GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM;

static uint32_t deleted_fragments = 0;


static void check_pads(GstElement *element) {
    GstIterator *iter = gst_element_iterate_pads(element);
    GValue *elem;
    
    while (gst_iterator_next(iter, elem) == GST_ITERATOR_OK) {
        gchar * strVal = g_strdup_value_contents (elem);
        DEBUG_TRACE("pad: " << strVal);
        free (strVal);
    }
    gst_iterator_free(iter);
}


static gboolean my_delete_fragment_callback(GstElement *element, const gchar *uri, gpointer user_data) {
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

    GstPad *hlssink_pad = gst_element_get_static_pad(hlssink, PAD_NAME);
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
    GstPad *hlssink_pad = gst_element_get_static_pad(hlssink, PAD_NAME);
    if(hlssink_pad) {
        DEBUG_TRACE("to block stream");
        ret = gst_pad_add_probe(hlssink_pad, GST_PAD_PROBE_TYPE_BLOCK_DOWNSTREAM, block_downstream_probe, NULL, NULL);
    }
    
    return ret;
}


int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstElement *pipeline, *videotestsrc, *x264enc, *hlssink;

    pipeline = gst_pipeline_new("hls-pipeline");
    videotestsrc = gst_element_factory_make("videotestsrc", "video-source");
    x264enc = gst_element_factory_make("x264enc", "video-encoder");
    hlssink = gst_element_factory_make("hlssink2", "hls-sink");

    if (!pipeline || !videotestsrc || !x264enc || !hlssink) {
        g_printerr("One or more elements could not be created. Exiting.\n");
        return -1;
    }

    g_object_set(videotestsrc, "pattern", 0, NULL); // Set the test pattern
    g_object_set(hlssink, "location", "/tmp/record_%05d.ts", NULL);
    g_object_set(hlssink, "playlist-location", "/tmp/playlist.m3u8", NULL);
    g_object_set(hlssink, "playlist-root", "/tmp", NULL);
    g_object_set(hlssink, "playlist-length", 20, NULL);
    g_object_set(hlssink, "max-files", 20, NULL);
    g_object_set(hlssink, "target-duration", 10, NULL);

    g_signal_connect(G_OBJECT(hlssink), "delete-fragment", G_CALLBACK(my_delete_fragment_callback), NULL);

    GstBus *bus = gst_element_get_bus(pipeline);

    gst_bin_add_many(GST_BIN(pipeline), videotestsrc, x264enc, hlssink, NULL);
    gst_element_link_many(videotestsrc, x264enc, hlssink, NULL);
    
    check_pads(hlssink);

    DEBUG_TRACE("start playing for 10s...");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    DEBUG_TRACE("sleep for 10s...\n");
    std::this_thread::sleep_for(std::chrono::seconds(10));

    gulong probe_id = stop_record(hlssink);
    DEBUG_TRACE("stop record for 10s...");
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    
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
    gst_object_unref(pipeline);
    gst_object_unref(bus);

    return 0;
}
