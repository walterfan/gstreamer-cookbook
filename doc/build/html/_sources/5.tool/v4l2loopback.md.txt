# v4l2loopback

## 安装

```
$ make && sudo make install
$ sudo depmod -a
```

装载模块

```
sudo modprobe v4l2loopback video_nr=12 max_buffers=2 exclusive_caps=1 card_label="VirtualCam"
```

卸载模块

```
sudo modprobe -r v4l2loopback
```

## 设置

```
v4l2loopback-ctl set-timeout-image -t 3000 /dev/video0 service-unavailable.png
```

## 示例

* play a mp4 file to a virtual camera

```
gst-launch-1.0 -v -m --gst-debug=5 filesrc location=talk.mp4 ! decodebin ! nvvideoconvert ! identity drop-allocation=1 ! v4l2sink device=/dev/video12 
```

* send video stream from local camera

```
gst-launch-1.0 v4l2src ! 'video/x-raw, width=640, height=480, \
framerate=30/1' ! videoconvert ! x264enc pass=qual quantizer=20 \
tune=zerolatency ! rtph264pay ! udpsink host=192.168.0.5 port=1234


gst-launch-1.0 udpsrc port=1234 ! "application/x-rtp, payload=127" ! \
rtph264depay ! avdec_h264 ! videoconvert ! xvimagesink

gst-launch-1.0 udpsrc port=1234 ! "application/x-rtp, payload=127" ! \
rtph264depay ! avdec_h264 ! videoconvert ! v4l2sink device=/dev/video1

```

## 参考
* https://github.com/umlaeute/v4l2loopback
* https://aweirdimagination.net/2020/07/12/virtual-web-cam-using-gstreamer-and-v4l2loopback/