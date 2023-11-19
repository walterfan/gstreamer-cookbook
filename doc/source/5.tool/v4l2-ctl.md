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
