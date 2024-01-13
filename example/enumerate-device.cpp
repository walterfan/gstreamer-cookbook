#include <gst/gst.h>

static const char* video_device_type="Video/Source";
static const char* video_caps_type="video/x-raw";
static const char* audio_device_type="Audio/Source";
static const char* audio_caps_type="audio/x-raw";

static gboolean exit_loop_callback(gpointer user_data) {
    GMainLoop *loop = (GMainLoop *)user_data;
    g_main_loop_quit(loop);
    return G_SOURCE_REMOVE;
}

static gboolean my_bus_func (GstBus * bus, GstMessage * message, gpointer user_data)
{
    GstDevice *device;
    gchar *name;

    switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_DEVICE_ADDED:
        gst_message_parse_device_added (message, &device);
        name = gst_device_get_display_name (device);
        g_print("Device added: %s\n", name);
        g_free (name);
        gst_object_unref (device);
        break;
    case GST_MESSAGE_DEVICE_REMOVED:
        gst_message_parse_device_removed (message, &device);
        name = gst_device_get_display_name (device);
        g_print("Device removed: %s\n", name);
        g_free (name);
        gst_object_unref (device);
        break;
    default:
        break;
    }

    return G_SOURCE_CONTINUE;
}

GstDeviceMonitor * setup_raw_device_monitor () 
{
    GstDeviceMonitor *monitor;
    GstBus *bus;
    GstCaps *video_caps, *audio_caps;

    monitor = gst_device_monitor_new ();

    bus = gst_device_monitor_get_bus (monitor);
    gst_bus_add_watch (bus, my_bus_func, NULL);
    gst_object_unref (bus);

    video_caps = gst_caps_new_empty_simple (video_caps_type);
    gst_device_monitor_add_filter (monitor, video_device_type, video_caps);

    audio_caps = gst_caps_new_empty_simple (audio_caps_type);
    gst_device_monitor_add_filter (monitor, audio_device_type, audio_caps);

    gst_caps_unref (video_caps);
    gst_caps_unref (audio_caps);

    gst_device_monitor_start (monitor);

    return monitor;
}


int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GstDeviceMonitor *monitor;
    GMainLoop *loop;
    guint timeout_id;

    // Create a device monitor
    monitor = setup_raw_device_monitor();
    if (!monitor) {
        g_print("Failed to create device monitor\n");
        return -1;
    }

    // Create and start the main loop
    loop = g_main_loop_new(NULL, FALSE);

    // Set a timeout to exit the loop after a specified duration (e.g., 5 seconds)
    timeout_id = g_timeout_add_seconds(5, exit_loop_callback, loop);

    g_main_loop_run(loop);

    // Remove the timeout source
    g_source_remove(timeout_id);

    // Clean up resources
    g_main_loop_unref(loop);
    gst_object_unref(monitor);

    return 0;
}
