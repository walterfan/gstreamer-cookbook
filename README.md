# GStreamer Cookbook

A cookbook for gstreamer developer - [GStreamer cookbook](https://walterfan.github.io/gstreamer-cookbook/)


## 新手 Primer
* [GStreamer 快速入门](doc/source/1.basic/basic.md)
* [GStreamer 基本概念](doc/source/1.basic/concept.md)
* [GStreamer 设计原则](doc/source/1.basic/design.rst)
* [GStreamer Element](doc/source/1.basic/gst-element.md)
* [GStreamer Bin](doc/source/1.basic/gst-bin.md)
* [GStreamer Bus](doc/source/1.basic/gst-bus.md)
* [GStreamer Pad](doc/source/1.basic/gst-pad.md)
* [GStreamer Caps](doc/source/1.basic/gst-caps.md)
* [GStreamer Buffer](doc/source/1.basic/gst-buffer.md)
* [GStreamer Event](doc/source/1.basic/gst-event.md)
* [GStreamer State](doc/source/1.basic/gst-state.md)
* [GStreamer pipeline](doc/source/3.application/pipeline_manipulation.md)
* [Audio pipeline](doc/source/3.application/audio_pipeline.md)
* [Video pipeline](doc/source/3.application/video_pipeline.md)
* [GStreamer FAQ](doc/source/3.application/faq.md)
* [GStreamer Debug](doc/source/3.application/debug.md)

## 进阶者 Advanced User

* [GStreamer plugin - boilerplate](doc/source/4.plugin/1.plugin-boilerplate.md)
* [GStreamer plugin - pads](doc/source/4.plugin/2.plugin-pads.md)
* [GStreamer plugin - chain function]
* [GStreamer plugin - event function]
* [GStreamer plugin - query function]
* [GStreamer plugin - signal](doc/source/4.plugin/7.plugin-signal.md)
* [GStreamer plugins](doc/source/4.plugin/index.rst)
* [GStreamer tools](doc/source/5.tool/index.rst)

* [GStreamer Probe]
* [GStreamer dynamic pads]
* [GStreamer scheduling modes]
* [GStreamer Caps Negotiation]
* [GStreamer Interfaces]
* [GStreamer metadata and stream info]

## 老兵 Veteran

> 老兵永远不死，只会慢慢凋零 Old soldiers never die, they just fade away

* [GStreamer memory allocation]
* [GStreamer media types and properties]
* [GStreamer event seeking]
* [GStreamer clocking]
* [GStreamer QoS]
* [GStreamer Dynamic parameters]

* [DeepStream get started](doc/source/3.application/deepstream.md)


## Quick test

```

gst-launch-1.0 -v -e filesrc location=material/talk.mp4 \
! decodebin ! videoconvert ! autovideosink

```

## Example
* start SRS by docker
```
export CANDIDATE="192.168.0.106"
sudo docker run --rm --env CANDIDATE=$CANDIDATE \
  -p 1935:1935 -p 1975:8080 -p 1985:1985 -p 1995:8000/udp \
  registry.cn-hangzhou.aliyuncs.com/ossrs/srs:5 \
  objs/srs -c conf/rtmp2rtc.conf
```

* push local video stream from mp4 to rtmp

```
gst-launch-1.0 -vv filesrc location=material/talk.mp4 \
! decodebin \
! videoconvert ! identity drop-allocation=1 \
! x264enc tune=zerolatency ! flvmux streamable=true \
! rtmpsink location='rtmp://192.168.0.106:1935/live/waltertest'
```

* or run the C++ program

refer to sub module [snippets](./snippets) for more C/C++ examples