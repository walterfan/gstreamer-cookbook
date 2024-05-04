# GStreamer State

GStreamer 的管道或者说元件有如下的状态转换:
 
状态转换: NULL -> READY -> PAUSED -> PLAYING

* NULL: This is the initial state of an element.

这是元素的初始状态

* READY: The element should be prepared to go to PAUSED.

这时已经准备好了，可以进入暂停状态

* PAUSED: The element should be ready to accept and process data. Sink elements, however, only accept one buffer and then block.

这时元素就可以接收和处理数据了，但是sink元件只能接受一个数据缓冲就会阻塞住

* PLAYING: The same as PAUSED except for live sources and sinks. Sinks accept and render data. Live sources produce data.

与 PAUSED 状态相同, 除了活动的 源和sink元件。Sink接受数据并渲染，live源产生


A sink is never set to PLAYING before it is prerolled. In order to do this, the pipeline (at the GstBin level) keeps track of all elements that require preroll (the ones that return ASYNC from the state change). These elements posted an ASYNC_START message without a matching ASYNC_DONE one.

The pipeline will not change the state of the elements that are still doing an ASYNC state change.

When an ASYNC element prerolls, it commits its state to PAUSED and posts an ASYNC_DONE message. The pipeline notices this ASYNC_DONE message and matches it with the ASYNC_START message it cached for the corresponding element.

When all ASYNC_START messages are matched with an ASYNC_DONE message, the pipeline proceeds with setting the elements to the final state again.

The base time of the element was already set by the pipeline when it changed the NO_PREROLL element to PLAYING. This operation has to be performed in the separate async state change thread (like the one currently used for going from PAUSED→PLAYING in a non-live pipeline).

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
