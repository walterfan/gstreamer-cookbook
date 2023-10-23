# splitmuxsink plugin
## Overview

splitmuxsink element wraps a muxer and a sink, and starts a new file when the mux contents are about to cross a threshold of maximum size of maximum time, splitting at video keyframe boundaries. Exactly one input video stream can be muxed, with as many accompanying audio and subtitle streams as desired.

By default, it uses mp4mux and filesink, but they can be changed via the 'muxer' and 'sink' properties.

The minimum file size is 1 GOP, however - so limits may be overrun if the distance between any 2 keyframes is larger than the limits.

If a video stream is available, the splitting process is driven by the video stream contents, and the video stream must contain closed GOPs for the output file parts to be played individually correctly. In the absence of a video stream, the first available stream is used as reference for synchronization.

In the async-finalize mode, when the threshold is crossed, the old muxer and sink is disconnected from the pipeline and left to finish the file asynchronously, and a new muxer and sink is created to continue with the next fragment. For that reason, instead of muxer and sink objects, the muxer-factory and sink-factory properties are used to construct the new objects, together with muxer-properties and sink-properties.


## Example pipelines
```
 gst-launch-1.0 -e v4l2src num-buffers=500 ! video/x-raw,width=320,height=240 ! videoconvert ! queue ! timeoverlay ! x264enc key-int-max=10 ! h264parse ! splitmuxsink location=video%02d.mov max-size-time=10000000000 max-size-bytes=1000000
```

Records a video stream captured from a v4l2 device and muxes it into ISO mp4 files, splitting as needed to limit size/duration to 10 seconds and 1MB maximum size.

```
 gst-launch-1.0 -e v4l2src num-buffers=500 ! video/x-raw,width=320,height=240 ! videoconvert ! queue ! timeoverlay ! x264enc key-int-max=10 ! h264parse ! splitmuxsink location=video%02d.mkv max-size-time=10000000000 muxer-factory=matroskamux muxer-properties="properties,streamable=true"
```

Records a video stream captured from a v4l2 device and muxer it into streamable Matroska files, splitting as needed to limit size/duration to 10 seconds. Each file will finalize asynchronously.

```
 gst-launch-1.0 videotestsrc num-buffers=10 ! jpegenc ! .video splitmuxsink muxer=qtmux muxer-pad-map=x-pad-map,video=video_1 location=test%05d.mp4 -v
```

Records 10 frames to an mp4 file, using a muxer-pad-map to make explicit mappings between the splitmuxsink sink pad and the corresponding muxer pad it will deliver to.

## Hierarchy
```
GObject
    ╰──GInitiallyUnowned
        ╰──GstObject
            ╰──GstElement
                ╰──GstBin
                    ╰──splitmuxsink
```