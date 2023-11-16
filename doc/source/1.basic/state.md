# GStreamer State

## What are states?
A state describes whether the element instance is initialized, whether it is ready to transfer data and whether it is currently handling data. There are four states defined in GStreamer:

* GST_STATE_NULL: Deactivated, element occupies no resources
* GST_STATE_READY:  Check and allocate resources
* GST_STATE_PAUSED: pre-roll, i.e. get a buffer to each sink
* GST_STATE_PLAYING: active dataflow, running-time is increasing

which will from now on be referred to simply as “NULL”, “READY”, “PAUSED” and “PLAYING”.

GST_STATE_NULL
----------------------

GST_STATE_NULL is the default state of an element. In this state, it has not allocated any runtime resources, it has not loaded any runtime libraries and it can obviously not handle data.


GST_STATE_READY
----------------------
GST_STATE_READY is the next state that an element can be in. In the READY state, an element has all default resources (runtime-libraries, runtime-memory) allocated. However, it has not yet allocated or defined anything that is stream-specific. When going from NULL to READY state (GST_STATE_CHANGE_NULL_TO_READY), an element should allocate any non-stream-specific resources and should load runtime-loadable libraries (if any). When going the other way around (from READY to NULL, GST_STATE_CHANGE_READY_TO_NULL), an element should unload these libraries and free all allocated resources. Examples of such resources are hardware devices. Note that files are generally streams, and these should thus be considered as stream-specific resources; therefore, they should not be allocated in this state.


GST_STATE_PAUSED
----------------------
GST_STATE_PAUSED is the state in which an element is ready to accept and handle data. For most elements this state is the same as PLAYING. The only exception to this rule are sink elements. Sink elements only accept one single buffer of data and then block. At this point the pipeline is 'prerolled' and ready to render data immediately.

GST_STATE_PLAYING
----------------------
GST_STATE_PLAYING is the highest state that an element can be in. For most elements this state is exactly the same as PAUSED, they accept and process events and buffers with data. Only sink elements need to differentiate between PAUSED and PLAYING state. In PLAYING state, sink elements actually render incoming data, e.g. output audio to a sound card or render video pictures to an image sink.

## Reference
* https://gstreamer.freedesktop.org/documentation/plugin-development/basics/states.html?gi-language=c
* https://gstreamer.freedesktop.org/documentation/additional/design/states.html?gi-language=c