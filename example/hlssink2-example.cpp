#include <gst/gst.h>
#include <glib.h>

#define GstHlsSink2DeleteFragmentResult int
#define GST_HLS_SINK2_DELETE_FRAGMENT_OK 0 

static GstHlsSink2DeleteFragmentResult
my_delete_fragment_callback(GstElement *element, const gchar *uri, gpointer user_data) {
    // Your custom logic for handling fragment deletion here.
    // In this example, we will simply print a message.
    g_print("Deleted fragment: %s\n", uri);
    // Return GST_HLS_SINK2_DELETE_FRAGMENT_OK on success.
    return GST_HLS_SINK2_DELETE_FRAGMENT_OK;
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
    g_object_set(hlssink, "playlist-length", 5, NULL);
    g_object_set(hlssink, "max-files", 5, NULL);
    g_object_set(hlssink, "target-duration", 10, NULL);

    g_signal_connect(G_OBJECT(hlssink), "delete-fragment", G_CALLBACK(my_delete_fragment_callback), NULL);

    GstBus *bus = gst_element_get_bus(pipeline);

    gst_bin_add_many(GST_BIN(pipeline), videotestsrc, x264enc, hlssink, NULL);
    gst_element_link_many(videotestsrc, x264enc, hlssink, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    if (msg) {
        g_print("Received message of type %s\n", GST_MESSAGE_TYPE_NAME(msg));
        gst_message_unref(msg);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_object_unref(bus);

    return 0;
}
