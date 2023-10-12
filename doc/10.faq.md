FAQ
===============
# What
## What state of a pipeline?

* NULL：element创建出来后的默认状态，没有分配任何资源。其他状态转换到此状态，会设防所有的资源。
* READY：就绪状态。element已分配了所有资源，还没有打开流。
* PAUSED：element已打开流，但还没对其进行处理。与PLAYING状态相比，只缺少时钟。
* PLAYING：element中的时钟开始运行，流不断通过pipeline

# How to
## How to Start Playing a Video at a Specific Point in Time with HLS?

Use the EXT-X-START tag. It’s optional, but if it’s present in a playlist it specifies the preferred point in the video to start playback.

The following attributes are defined in the spec for the EXT-X-START tag:

* TIME-OFFSET – (required) The offset in seconds to start playing the video. If the value is negative, specifies the time from the end of the playlist. The offset must not be larger than the playlist duration.
* PRECISE – (optional) Valid string values: YES, NO. If YES, playback starts at the point specified by TIME-OFFSET but does not render frames in the segment prior to the offset. Default is NO.

例如

```
#EXT-X-START:TIME-OFFSET=25,PRECISE=YES
```

## How to set width and height of video

```
gst-launch-1.0 videotestsrc ! video/x-raw, format=BGR ! autovideoconvert ! videoconvert ! video/x-raw, width=640, height=480, framerate=1/2 ! ximagesink
```


## How to send video from one host over udp?

### test video
* sender
```
gst-launch-1.0 -v videotestsrc ! x264enc ! rtph264pay pt=96 ! udpsink host=192.168.104.214 port=5000
```

* receiver
```
gst-launch-1.0 -v udpsrc port=500 caps=application/x-rtp ! rtph264depay ! avdec_h264 ! autovideosink
```

## How to capture video from desktop

refer to https://github.com/xmementoit/gstreamerCheatsheet/blob/master/README.md

## How to capture video from camera to 264 file

```
gst-launch-1.0 v4l2src device=/dev/video2 ! nvvidconv ! 'video/x-raw(memory:NVMM),format=NV12,width=640,height=360,fps=30' ! nvv4l2h264enc insert-sps-pps=true bitrate=1000000 ! filesink location=./test.264
```

## How to play HLS stream?
```
 gst-launch-1.0 souphttpsrc location=http://devimages.apple.com/iphone/samples/bipbop/gear4/prog_index.m3u8 ! hlsdemux ! decodebin ! videoconvert ! videoscale ! autovideosink
```

or use https://github.com/videojs/http-streaming

refer to https://gstreamer.freedesktop.org/documentation/hls/hlsdemux.html?gi-language=c

1) 在本地生成 playlist.m3u8 和 TS 文件

```
 gst-launch-1.0 videotestsrc is-live=true ! x264enc ! h264parse ! hlssink2 max-files=5
```
2) 启动 web server： `python3 -m http.server`

3) 播放 HLS m3u8 和相应的 TS 

```
gst-play-1.0 http://localhost:8000/playlist.m3u8 --use-playbin
```

或者

```
gst-launch-1.0 souphttpsrc location=http://localhost:8000/playlist.m3u8 ! hlsdemux ! decodebin ! videoconvert ! videoscale ! autovideosink
```

### camera
* Sender

```
gst-launch-1.0 -v v4l2src device=/dev/video1 ! decodebin ! videoconvert ! omxh264enc ! video/x-h264,stream-format=byte-stream ! rtph264pay ! udpsink host=192.168.104.214 port=5000

```
* receiver

```

gst-launch-1.0 -v udpsrc  port=5000 caps=application/x-rtp ! rtph264depay ! avdec_h264 ! autovideosink
```

## How to make a gstreamer plugin

1) build develop environment

.. code-block::

   docker run --name gst_dev --rm -i -t -v `pwd`:/workspace restreamio/gstreamer:latest-dev-with-source /bin/bash

2) 定义存储数据的元素的结构
3) 定义这个元素的类
4) 定义这个元素的标准宏
5) 定义返回类型信息的标准函数
6) 注册这个元素


Reference
==============
* GStreamer document: https://gitlab.freedesktop.org/gstreamer/gst-docs.git
* GStreamer plugin guide: https://gstreamer.freedesktop.org/documentation/plugin-development/index.html?gi-language=c

* https://github.com/DamZiobro/gstreamerCheatsheet


