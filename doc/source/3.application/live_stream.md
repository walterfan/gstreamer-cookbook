# Live Stream

1) 启动 SRS 5

```bash
export CANDIDATE="192.168.104.249"
sudo docker run --rm -itd --env CANDIDATE=$CANDIDATE \
  -p 1935:1935 -p 1980:8080 -p 1985:1985 -p 8000:8000/udp \
  registry.cn-hangzhou.aliyuncs.com/ossrs/srs:5 \
  objs/srs -c conf/rtmp2rtc.conf
```

2) 推送视频流到 SRS 上

```bash
gst-launch-1.0 -vv filesrc location=digits.mp4 ! decodebin ! nvvideoconvert \
! identity drop-allocation=1 ! x264enc tune=zerolatency \
! flvmux streamable=true ! rtmpsink location='rtmp://192.168.104.249:1935/live/waltertest'
```

3) 通过 SRS 来播放视频流


```
- http://192.168.104.249:8080/console/  flv.js
```


## HLS

* playback TS file
```bash
gst-launch-1.0 filesrc location=xxx.ts ! tsdemux ! queue ! h264parse ! openh264dec ! autovideosink

```

### send and receive TS, and playback it
* send TS over udp

```bash
gst-launch-1.0 filesrc location=xxx.ts ! tsparse set-timestamps=true ! video/mpegts ! tsdemux ! video/x-h264 ! h264parse disable-passthrough=true ! rtph264pay ! udpsink -v host=127.0.0.1 port=9999

```

* receive TS over udp

```bash
gst-launch-1.0 udpsrc port=9999 caps="application/x-rtp, media=video, clock-rate=90000, encoding-name=H264, payload=96" ! queue ! rtph264depay ! decodebin ! videoconvert ! glimagesink
```


## Reference
* https://github.com/matthew1000/gstreamer-cheat-sheet/blob/master/rtmp.md