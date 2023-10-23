# v4l2src plugin
## Overview

v4l2src 用来在 linux 系统中从视频采集设备中捕捉视频流的常用插件

参见 https://gstreamer.freedesktop.org/documentation/video4linux2/v4l2src.html?gi-language=c

## Hierarchy

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstBaseSrc
                    ╰──GstPushSrc
                        ╰──v4l2src

```

## Example


* This pipeline shows the video captured from /dev/video0 tv card and for webcams.

```
gst-launch-1.0 v4l2src ! xvimagesink
```

* This pipeline shows the video captured from a webcam that delivers jpeg images.

```
gst-launch-1.0 v4l2src ! jpegdec ! xvimagesink
```