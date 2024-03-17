# GST Tracer

It provides a mechanism to get structured tracing info from GStreamer applications. This can be used for post-run analysis as well as for live introspection.


```sh

env GST_DEBUG="GST_TRACER:7" \
    GST_TRACERS="latency(flags=element+pipeline)" \ 
    GST_DEBUG_FILE=./latency.log \
    gst-launch-1.0 -v videotestsrc pattern=snow ! video/x-raw,width=640,height=480,framerate=30/1 ! videoconvert ! autovideosink

```    