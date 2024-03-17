# Video Pipeline

## samples
### check video source

```sh
gst-launch-1.0 -ev v4l2src device=/dev/video0 num-buffers=1 ! jpegenc ! filesink location=videotest.jpg

gst-launch-1.0 v4l2src device=/dev/video1 do-timestamp=true num-buffers=10 ! image/jpeg,width=1920,height=1080,framerate=20/1 ! jpegparse ! multifilesink location=videotest_%03d.jpeg

```


### play camera video

```sh
gst-launch-1.0 -ev v4l2src device=/dev/video1 num-buffers=100 ! image/jpeg,width=1920,height=1080,framerate=20/1 ! xvimagesink

```


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

### record and display at the same t e(queue)

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

gst-launch-1.0 -ev nvarguscamerasrc num-buffers=2000 ! 'video/x-raw(memory:NVMM),width=1920, height=1080, framerate=30/1, format=NV12' ! omxh264enc ! qtmux ! filesink location=test.mp4

gst-launch-1.0 -ev v4l2src device=/dev/video0 num-buffers=100 ! capsfilter caps='video/x-raw,width=1920, height=1080, framerate=60/1' ! nvvideoconvert ! videorate ! capsfilter caps='video/x-raw,width=1920, height=1080, framerate=20/1' ! nvv4l2h264enc ! h264parse ! mp4mux ! filesink location=test.mp4                  



gst-launch-1.0 -ev v4l2src device=/dev/video0 num-buffers=100 ! capsfilter caps='video/x-raw,width=1920, height=1080, framerate=60/1' ! nvvideoconvert ! videorate max-rate=30 drop-only=1 ! nvv4l2h264enc ! h264parse ! mp4mux ! filesink location=test.mp4 


gst-launch-1.0 -ev v4l2src device=/dev/video0 num-buffers=100 ! capsfilter caps='video/x-raw,width=1920, height=1080, framerate=60/1' ! nvvideoconvert ! videorate max-rate=30 ! capsfilter caps='video/x-raw,framerate=30/1' ! nvv4l2h264enc ! h264parse ! mp4mux ! filesink location=test.mp4 

gst-launch-1.0 -ev v4l2src device=/dev/video0 num-buffers=100 ! capsfilter caps='video/x-raw,width=1920, height=1080, framerate=60/1' ! nvvideoconvert ! videorate max-rate=30 ! nvv4l2h264enc ! h264parse ! mp4mux ! filesink location=test.mp4 
```

### fps test
```sh
gst-launch-1.0 -v videotestsrc ! videorate ! video/x-raw,framerate=30/1 ! videoconvert ! autovideosink

gst-launch-1.0 -v videotestsrc ! videorate ! video/x-raw,framerate=30/1 ! x264enc tune=zerolatency bitrate=16000000 speed-preset=superfast ! rtph264pay ! udpsink port=5000 host=$HOST

gst-launch-1.0 -v videotestsrc ! videorate ! video/x-raw,framerate=60/1 ! x264enc tune=zerolatency bitrate=16000000 speed-preset=superfast ! h264parse ! rtph264pay ! udpsink port=5000 host=$HOST

gst-launch-1.0 -v videotestsrc ! video/x-raw,framerate=30/1 ! videorate ! video/x-raw,framerate=60/1 ! x264enc tune=zerolatency bitrate=16000000 speed-preset=superfast ! rtph264pay ! udpsink port=5000 host=$HOST
```

### capture screen and display the video

```sh
gst-launch-1.0 ximagesrc startx=160 endx=1080 use-damage=0 ! video/x-raw,framerate=30/1 ! videoscale method=0 ! video/x-raw,width=640,height=480  ! ximagesink

```

### read mp4 and demux to audio and video

```sh
gst-launch-1.0 filesrc location=talk.mp4 ! decodebin name=dec ! videoconvert ! autovideosink dec. ! audioconvert ! audioresample ! autoaudiosink
```sh

### add a clock on the above video

```sh
gst-launch-1.0 filesrc location=talk.mp4 ! decodebin name=dec ! videoconvert ! clockoverlay ! autovideosink dec. ! audioconvert ! audioresample ! autoaudiosink

```

### send/receive video stream over udp

```sh
# send video

gst-launch-1.0 -v v4l2src device=/dev/video1 ! decodebin \
  ! videoconvert ! omxh264enc ! video/x-h264,stream-format=byte-stream \
  ! rtph264pay ! udpsink host=192.168.104.236 port=5000

# receive video

gst-launch-1.0 -v udpsrc  port=5000 caps=application/x-rtp \
  ! rtph264depay ! avdec_h264 ! autovideosink

```