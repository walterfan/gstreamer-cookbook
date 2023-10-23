# queue plugin
## Overview

[queue plugin](https://gstreamer.freedesktop.org/documentation/coreelements/queue.html?gi-language=c) 


Data is queued until one of the limits specified by the max-size-buffers, max-size-bytes and/or max-size-time properties has been reached. Any attempt to push more buffers into the queue will block the pushing thread until more space becomes available.

The queue will create a new thread on the source pad to decouple the processing on sink and source pad.

You can query how many buffers are queued by reading the current-level-buffers property. You can track changes by connecting to the notify::current-level-buffers signal (which like all signals will be emitted from the streaming thread). The same applies to the current-level-time and current-level-bytes properties.

The default queue size limits are 200 buffers, 10MB of data, or one second worth of data, whichever is reached first.

As said earlier, the queue blocks by default when one of the specified maximums (bytes, time, buffers) has been reached. You can set the leaky property to specify that instead of blocking it should leak (drop) new or old buffers.

The underrun signal is emitted when the queue has less data than the specified minimum thresholds require (by default: when the queue is empty). The overrun signal is emitted when the queue is filled up. Both signals are emitted from the context of the streaming thread.



## Hierarchy
```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──queue
```