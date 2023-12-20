# Video Pipeline

## samples
### display test video

```sh
gst-launch-1.0 -vvv videotestsrc ! autovideosink
gst-launch-1.0 -vvv videotestsrc ! 'video/x-raw,width=1280,height=720,format=RGB,framerate=60/1' ! fpsdisplaysink
gst-launch-1.0 -vvv videotestsrc ! videoconvert ! fpsdisplaysink text-overlay=false
```

### record to file
```sh

# record video to flv
gst-launch-1.0 -v -e videotestsrc ! x264enc ! flvmux ! filesink location=xyz.flv

# record vidoe to mp4
gst-launch-1.0 -v -e videotestsrc ! x264enc ! qtmux ! filesink location=xyz.mp4

# record video to m3u8 and ts
gst-launch-1.0 videotestsrc is-live=true ! x264enc ! h264parse ! hlssink2 max-files=5
```

### record and display at the same time(queue)

[GStreamer Recording and Viewing Stream Simultaneously](https://stackoverflow.com/questions/37444615/gstreamer-recording-and-viewing-stream-simultaneously)

```sh
gst-launch-1.0 -vvv videotestsrc \
! tee name=t \
t. ! queue ! x264enc ! mp4mux ! filesink location=xyz.mp4 -e \
t. ! queue leaky=1 ! autovideosink sync=false
```

**tips:**

1. `-e (EOS signal)`: Pipelines for file saving require a reliable EOS(End of Stream) signal
2. `queue leaky=1 ! autovideosink sync=false`: prevent blocking 
3. `drop=true`: drop frame if cannot read quickly enough


### record webcam to `*.mp4`(jetson nano)

```sh
gst-launch-1.0 nvarguscamerasrc ! fakesink

gst-launch-1.0 nvarguscamerasrc num-buffers=2000 ! 'video/x-raw(memory:NVMM),width=1920, height=1080, framerate=30/1, format=NV12' ! omxh264enc ! qtmux ! filesink location=test.mp4 -e

gst-launch-1.0 nvarguscamerasrc num-buffers=2000 ! 'video/x-raw(memory:NVMM),width=1920, height=1080, framerate=30/1, format=NV12' ! nvvidconv ! x264enc ! qtmux ! filesink location=test.mp4 -e

```

### fps test
```sh
gst-launch-1.0 -v videotestsrc ! videorate ! video/x-raw,framerate=30/1 ! videoconvert ! autovideosink

gst-launch-1.0 -v videotestsrc ! videorate ! video/x-raw,framerate=30/1 ! x264enc tune=zerolatency bitrate=16000000 speed-preset=superfast ! rtph264pay ! udpsink port=5000 host=$HOST

gst-launch-1.0 -v videotestsrc ! videorate ! video/x-raw,framerate=60/1 ! x264enc tune=zerolatency bitrate=16000000 speed-preset=superfast ! h264parse ! rtph264pay ! udpsink port=5000 host=$HOST

gst-launch-1.0 -v videotestsrc ! video/x-raw,framerate=30/1 ! videorate ! video/x-raw,framerate=60/1 ! x264enc tune=zerolatency bitrate=16000000 speed-preset=superfast ! rtph264pay ! udpsink port=5000 host=$HOST
```