#include <gst/gst.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

static std::string bytes_to_hex(uint8_t* bytes, size_t len) {
    std::stringstream ss;

    // Ensure the output is in uppercase and has two characters for each byte
    ss << std::uppercase << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << std::hex << static_cast<int>(bytes[i]);
    }

    return ss.str();
}

static gboolean handoff_callback(GstElement *sink, GstBuffer *buffer, gpointer user_data)
{
  // This function will be called when the handoff signal is emitted
  g_print("Handoff Callback - Received buffer with size: %zu\n", gst_buffer_get_size(buffer));
  return TRUE;
}

GstPadProbeReturn cb_have_data_list(GstPad *pad, GstPadProbeInfo *info,
                                    gpointer user_data)
{

    GstBufferList *buflist = gst_pad_probe_info_get_buffer_list(info);
    if (buflist)
        g_print("cb_hava_data_list from %s\n", (char *)user_data);
    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn cb_have_data(GstPad *pad, GstPadProbeInfo *info,
                                gpointer user_data)
{

    //GstBuffer *buf = gst_pad_probe_info_get_buffer(info);
    GstBuffer *buf = (GstBuffer *) info->data;
    GstMapInfo in_map_info;
    memset (&in_map_info, 0, sizeof (in_map_info));
    static uint32_t data_count = 0;

    if (buf) {
      g_print("cb_hava_data: offet=%lu from %s\n", buf->offset,
              (char *)user_data);
      if (gst_buffer_map (buf, &in_map_info, GST_MAP_READWRITE))
      {
        std::string hexstr = bytes_to_hex(in_map_info.data, in_map_info.size);
        std::cout << "cb_hava_data, count=" <<(++data_count) << ", size=" << in_map_info.size;
        if (data_count % 5 == 0) {
          std::cout << "receive data=" << hexstr << std::endl;
        }
        
      }
    }
    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn cb_have_event(GstPad *pad, GstPadProbeInfo *info,
                                    gpointer user_data)
{

    GstEvent *event = gst_pad_probe_info_get_event(info);
    if (event)
      g_print("cb_hava_event: event type=%s from %s\n", GST_EVENT_TYPE_NAME(event),
              (char *)user_data);

    return GST_PAD_PROBE_OK;
}

GstPadProbeReturn cb_have_query(GstPad *pad, GstPadProbeInfo *info,
                                gpointer user_data)
{

    GstQuery *query = gst_pad_probe_info_get_query(info);

    g_print("cb_hava_query: query type=%s from %s\n", GST_QUERY_TYPE_NAME(query),
          (char *)user_data);

    return GST_PAD_PROBE_OK;
}

void add_pad_probe(GstPad *pad_to_probe, const char *evt_pad_name)
{
    // GST_PAD_PROBE_TYPE_BUFFER_LIST
    gulong probe_id_0 = gst_pad_add_probe(pad_to_probe,
                                          GST_PAD_PROBE_TYPE_BUFFER_LIST, cb_have_data_list,
                                          (gpointer)evt_pad_name, NULL);

    // GST_PAD_PROBE_TYPE_BUFFER
    gulong probe_id_1 = gst_pad_add_probe(pad_to_probe,
                                          GST_PAD_PROBE_TYPE_BUFFER, cb_have_data, (gpointer)evt_pad_name,
                                          NULL);

    // GST_PAD_PROBE_TYPE_EVENT_BOTH
    gulong probe_id_2 = gst_pad_add_probe(pad_to_probe,
                                          GST_PAD_PROBE_TYPE_EVENT_BOTH, cb_have_event,
                                          (gpointer)evt_pad_name, NULL);

    // GST_PAD_PROBE_TYPE_QUERY_BOTH
    gulong probe_id_3 = gst_pad_add_probe(pad_to_probe,
                                          GST_PAD_PROBE_TYPE_QUERY_BOTH, cb_have_query,
                                          (gpointer)evt_pad_name, NULL);

}

int main(int argc, char *argv[])
{

  GstElement *pipeline, *source, *sink;
  GstBus *bus;
  GstMessage *msg;
  GstStateChangeReturn ret;

  gst_init(&argc, &argv);

  /* Create the elements */
  source = gst_element_factory_make("audiotestsrc", "source");
  sink = gst_element_factory_make("fakesink", "sink");

  /* Create the empty pipeline */
  pipeline = gst_pipeline_new("test-pipeline");

  if (!pipeline || !source || !sink)
  {
    g_printerr("Not all elements could be created.\n");
    return -1;
  }

  /* Build the pipeline */
  gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
  if (gst_element_link(source, sink) != TRUE)
  {
    g_printerr("Elements could not be linked.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  /* Modify the source's properties */
  g_object_set(source, "num-buffers", 10, NULL);
  g_object_set(source, "wave", 5, NULL);

  // Connect the handoff signal to the callback function
  g_signal_connect(sink, "handoff", G_CALLBACK(handoff_callback), NULL);

  GstPad* audioSinkPad = gst_element_get_static_pad(sink, "sink");
  add_pad_probe(audioSinkPad, "fakesink");

  /* Start playing */
  ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
  if (ret == GST_STATE_CHANGE_FAILURE)
  {
    g_printerr("Unable to set the pipeline to the playing state.\n");
    gst_object_unref(pipeline);
    return -1;
  }

  /* Wait until error or EOS */
  bus = gst_element_get_bus(pipeline);
  msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                 (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

  /* Parse message */
  if (msg != NULL)
  {
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error(msg, &err, &debug_info);
      g_printerr("Error received from element %s: %s\n",
                 GST_OBJECT_NAME(msg->src), err->message);
      g_printerr("Debugging information: %s\n",
                 debug_info ? debug_info : "none");
      g_clear_error(&err);
      g_free(debug_info);
      break;
    case GST_MESSAGE_EOS:
      g_print("End-Of-Stream reached.\n");
      break;
    default:
      /* We should not reach here because we only asked for ERRORs and EOS */
      g_printerr("Unexpected message received.\n");
      break;
    }
    gst_message_unref(msg);
  }

  /* Free resources */
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
  return 0;
}
