# v4l-utils

* 安装 
```
sudo apt-get install v4l-utils
```

1. 列出设备
```
v4l2-ctl --list-devices
```

2. 列出设备支持的格式

```
v4l2-ctl -d /dev/video1 --list-formats-ext 
```


3. List supported video formats and resolutions of a specific video device:
```
# v4l2-ctl --list-formats-ext --device path/to/video_device
```

4. Get all details of a video device:
```
# v4l2-ctl --all --device path/to/video_device
```

5. Capture a JPEG photo with a specific resolution from video device:
```
# v4l2-ctl --device path/to/video_device --set-fmt-video=width=width,height=height,pixelformat=MJPG --stream-mmap --stream-to=path/to/output.jpg --stream-count=1
```

6. Capture a raw video stream from video device:
```
# v4l2-ctl --device path/to/video_device --set-fmt-video=width=width,height=height,pixelformat=format --stream-mmap --stream-to=path/to/output --stream-count=number_of_frames_to_capture
```

7. List all video device’s controls and their values:
```
# v4l2-ctl --list-ctrls --device /path/to/video_device
```
# Gst Python

https://gist.github.com/liviaerxin/9934a5780f5d3fe5402d5986fc32d070

# GStreamer Tools
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

# 如何在命令行下建立一个pipeline

```
gst-launch-1.0 -v v4l2src device=/dev/video0 ! 'video/x-raw,width=640,height=480,framerate=15/1' ! fakesink

gst-launch-1.0 -vv v4l2src device=/dev/video0 num-buffers=1 ! "video/x-raw,format=YUY2,width=1280,height=720" ! jpegenc ! filesink location=filename.jpg
```

# 如何找出一个element的Capabilities

# 如何发现一个媒体文件的内部结构