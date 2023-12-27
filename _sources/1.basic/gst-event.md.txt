# GstEvent
The event class provides factory methods to construct events for sending and functions to query (parse) received events.

Events are usually created with `gst_event_new_*()` which takes event-type specific parameters as arguments. 

To send an event application will usually use `gst_element_send_event` and elements will use `gst_pad_send_event` or `gst_pad_push_event`. The event should be unreffed with gst_event_unref if it has not been sent.

Events that have been received can be parsed with their respective `gst_event_parse_*()` functions. It is valid to pass NULL for unwanted details.

Events are passed between elements in parallel to the data stream. Some events are serialized with buffers, others are not. Some events only travel downstream, others only upstream. Some events can travel both upstream and downstream.

The events are used to signal special conditions in the datastream such as EOS (end of stream) or the start of a new stream-segment. Events are also used to flush the pipeline of any pending data.

Most of the event API is used inside plugins. Applications usually only construct and use seek events. To do that `gst_event_new_seek` is used to create a seek event. It takes the needed parameters to specify seeking time and mode

## example

```c

   GstEvent *event;
   gboolean result;
   ...
   // construct a seek event to play the media from second 2 to 5, flush
   // the pipeline to decrease latency.
   event = gst_event_new_seek (1.0,
      GST_FORMAT_TIME,
      GST_SEEK_FLAG_FLUSH,
      GST_SEEK_TYPE_SET, 2 * GST_SECOND,
      GST_SEEK_TYPE_SET, 5 * GST_SECOND);
   ...
   result = gst_element_send_event (pipeline, event);
   if (!result)
     g_warning ("seek failed");
   ...
   
```

## Reference
* https://gstreamer.freedesktop.org/documentation/gstreamer/gstevent.html?gi-language=c