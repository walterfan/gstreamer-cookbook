# gst bin


## Overview

GstBin is an element that can contain other GstElement, allowing them to be managed as a group. Pads from the child elements can be ghosted to the bin, see GstGhostPad. This makes the bin look like any other elements and enables creation of higher-level abstraction elements.

A new GstBin is created with `gst_bin_new`. Use a GstPipeline instead if you want to create a toplevel bin because a normal bin doesn't have a bus or handle clock distribution of its own.

After the bin has been created you will typically add elements to it with `gst_bin_add`. You can remove elements with `gst_bin_remove`.

An element can be retrieved from a bin with `gst_bin_get_by_name`, using the elements name. `gst_bin_get_by_name_recurse_up ` is mainly used for internal purposes and will query the parent bins when the element is not found in the current bin.

An iterator of elements in a bin can be retrieved with gst_bin_iterate_elements. Various other iterators exist to retrieve the elements in a bin.

`gst_object_unref` is used to drop your reference to the bin.

The element-added signal is fired whenever a new element is added to the bin. Likewise the element-removed signal is fired whenever an element is removed from the bin.


## 消息处理

A GstBin internally intercepts every GstMessage posted by its children and implements the following default behaviour for each of them:

* GST_MESSAGE_EOS: This message is only posted by sinks in the PLAYING state. If all sinks posted the EOS message, this bin will post and EOS message upwards.

* GST_MESSAGE_SEGMENT_START: Just collected and never forwarded upwards. The messages are used to decide when all elements have completed playback of their segment.

* GST_MESSAGE_SEGMENT_DONE: Is posted by GstBin when all elements that posted a SEGMENT_START have posted a SEGMENT_DONE.

* GST_MESSAGE_DURATION_CHANGED: Is posted by an element that detected a change in the stream duration. The duration change is posted to the application so that it can refetch the new duration with a duration query.

  - Note that these messages can be posted before the bin is prerolled, in which case the duration query might fail.

  - Note also that there might be a discrepancy (due to internal buffering/queueing) between the stream being currently displayed and the returned duration query.

Applications might want to also query for duration (and changes) by listening to the GST_MESSAGE_STREAM_START message, signaling the active start of a (new) stream.

*  GST_MESSAGE_CLOCK_LOST: This message is posted by an element when it can no longer provide a clock.

The default bin behaviour is to check if the lost clock was the one provided by the bin. If so and the bin is currently in the PLAYING state, the message is forwarded to the bin parent.

This message is also generated when a clock provider is removed from the bin. If this message is received by the application, it should PAUSE the pipeline and set it back to PLAYING to force a new clock distribution.

* GST_MESSAGE_CLOCK_PROVIDE: This message is generated when an element can provide a clock. This mostly happens when a new clock provider is added to the bin.

The default behaviour of the bin is to mark the currently selected clock as dirty, which will perform a clock recalculation the next time the bin is asked to provide a clock.

This message is never sent to the application but is forwarded to the parent of the bin.

* OTHERS: posted upwards.


## 查询处理

A GstBin implements the following default behaviour for answering to a GstQuery:

* GST_QUERY_DURATION: The bin will forward the query to all sink elements contained within and will return the maximum value. If no sinks are available in the bin, the query fails.

* GST_QUERY_POSITION: The query is sent to all sink elements in the bin and the MAXIMUM of all values is returned. If no sinks are available in the bin, the query fails.

* OTHERS: the query is forwarded to all sink elements, the result of the first sink that answers the query successfully is returned. If no sink is in the bin, the query fails.

## 事件处理

A GstBin will by default forward any event sent to it to all sink ( GST_EVENT_TYPE_UPSTREAM ) or source ( GST_EVENT_TYPE_DOWNSTREAM ) elements depending on the event type.

If all the elements return TRUE, the bin will also return TRUE, else FALSE is returned. If no elements of the required type are in the bin, the event handler will return TRUE.


## 继承结构

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstBin
                    ╰──GstPipeline
```


## Members

* element (GstElement) – No description available
* numchildren (gint) – the number of children in this bin
* children (GList *) – the list of children in this bin
* children_cookie (guint32) – updated whenever children changes
* child_bus (GstBus *) – internal bus for handling child messages
* messages (GList *) – queued and cached messages
* polling (gboolean) – the bin is currently calculating its state
* state_dirty (gboolean) – the bin needs to recalculate its state (deprecated)
* clock_dirty (gboolean) – the bin needs to select a new clock
* provided_clock (GstClock *) – the last clock selected
* clock_provider (GstElement *) – the element that provided provided_clock