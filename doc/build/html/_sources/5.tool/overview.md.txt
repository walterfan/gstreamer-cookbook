# GStreamer Tools

## command line tools
* gst-device-monitor-1.0
* gst-inspect-1.0
* gst-play-1.0
* gst-typefind-1.0 
* gst-validate-launcher
* gst-validate-rtsp-server-1.0
* gst-discoverer-1.0 
* gst-launch-1.0 
* gst-shell 
* gst-validate-1.0
* gst-validate-media-check-1.0
* gst-validate-transcoding-1.0


## example
* 如何在命令行下建立一个pipeline

```
gst-launch-1.0 -v v4l2src device=/dev/video0 ! 'video/x-raw,width=640,height=480,framerate=15/1' ! fakesink

gst-launch-1.0 -vv v4l2src device=/dev/video0 num-buffers=1 ! "video/x-raw,format=YUY2,width=1280,height=720" ! jpegenc ! filesink location=filename.jpg
```

