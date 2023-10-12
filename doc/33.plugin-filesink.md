# Overview

filesink is to write incoming data to a file in the local file system.

# Example

```
 gst-launch-1.0 v4l2src num-buffers=1 ! jpegenc ! filesink location=capture1.jpeg
```

Capture one frame from a v4l2 camera and save as jpeg image.

# Hierarchy

```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstBaseSink
                    ╰──filesink
```