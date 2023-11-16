# v4l2sink plugin
## Overview

v4l2sink 可用于向 v4l2 设备显示视频（由图形硬件、电视输出等提供的屏幕覆盖）

## Hierarchy

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstBaseSink
                    ╰──GstVideoSink
                        ╰──v4l2sink

```

## Examples

## example 1

```
gst-launch-1.0 videotestsrc ! v4l2sink device=/dev/video1
```

This pipeline displays a test pattern on /dev/video1

## example 2
```
 gst-launch-1.0 -v videotestsrc ! navigationtest ! v4l2sink
```

A pipeline to test navigation events. While moving the mouse pointer over the test signal you will see a black box following the mouse pointer. If you press the mouse button somewhere on the video and release it somewhere else a green box will appear where you pressed the button and a red one where you released it. (The navigationtest element is part of gst-plugins-good.) You can observe here that even if the images are scaled through hardware the pointer coordinates are converted back to the original video frame geometry so that the box can be drawn to the correct position. This also handles borders correctly, limiting coordinates to the image area