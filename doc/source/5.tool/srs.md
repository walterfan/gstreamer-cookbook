# SRS

## 快速开始


1935 RTMP的常用端口
1985 API接口端口
8080 默认控制台访问端口


## 构建

```
brew install autoconf
brew install automake

git clone https://github.com/ossrs/srs
cd srs/trunk
./configure && make
```


## 配置
```
# conf/realtime.conf
listen              1935;
max_connections     1000;
vhost __defaultVhost__ {
    tcp_nodelay     on;
    min_latency     on;

    play {
        gop_cache       off;
        queue_length    10;
        mw_latency      100;
    }

    publish {
        mr off;
    }
}

```

## 启动

```
./objs/srs -c conf/realtime.conf

```

也可以通过 docker 启动

```
export CANDIDATE="192.168.0.104"
docker run --rm --env CANDIDATE=$CANDIDATE \
  -p 1935:1935 -p 1975:8080 -p 1985:1985 -p 8000:8000/udp \
  registry.cn-hangzhou.aliyuncs.com/ossrs/srs:5 \
  objs/srs -c conf/rtmp2rtc.conf
```


## 用例
### 推流
#### 推视频流
```sh
gst-launch-1.0 -v -e videotestsrc ! clockoverlay ! queue ! videoconvert ! x264enc tune=zerolatency ! flvmux streamable=true ! rtmpsink location='rtmp://192.168.0.104:1935/live/waltertest'
```

在我的 macbook 上，通过以下命令从摄像头中捕获视频流，发送到 SRS 上

```sh
gst-launch-1.0 -v -m avfvideosrc device-index=0 ! "video/x-raw,width=1280,height=720" ! queue ! videoconvert ! x264enc tune=zerolatency ! flvmux streamable=true ! rtmpsink location='rtmp://192.168.104.214:1935/live/macvideo'
```

note: x264enc parameters: speed-preset=superfast tune=zerolatency psy-tune=grain sync-lookahead=5 bitrate=480 key-int-max=50 ref=2 


To send a video test source:

```sh
export RTMP_DEST="rtmp://192.168.104.199:1935/live/teststream"

gst-launch-1.0 videotestsrc  is-live=true ! \
    queue ! x264enc ! flvmux name=muxer ! rtmpsink location="$RTMP_DEST live=1"

```

#### 推音频流

* send audio stream to rtpmp

```sh

export RTMP_DEST="rtmp://192.168.104.199:1935/live/testaudio"

gst-launch-1.0 audiotestsrc is-live=true ! \
    audioconvert ! audioresample ! audio/x-raw,rate=48000 ! \
    voaacenc bitrate=96000 ! audio/mpeg ! aacparse ! audio/mpeg, mpegversion=4 ! \
    flvmux name=mux ! \
    rtmpsink location=$RTMP_DEST


export RTMP_DEST="rtmp://192.168.104.199:1935/live/testaudio"
gst-launch-1.0 alsasrc device=hw:2,0 ! audioconvert ! wavescope ! videoconvert \
  ! x264enc ! flvmux name=muxer ! rtmpsink location="$RTMP_DEST live=1"

```


* To send an audio test source (note: flvmux is still required even though there is no muxing of audio & video):

```sh
gst-launch-1.0 audiotestsrc is-live=true ! \
    audioconvert ! audioresample ! audio/x-raw,rate=48000 ! \
    voaacenc bitrate=96000 ! audio/mpeg ! aacparse ! audio/mpeg, mpegversion=4 ! \
    flvmux name=mux ! \
    rtmpsink location=$RTMP_DEST
```

This sends both video and audio as a test source:

```sh
gst-launch-1.0 videotestsrc is-live=true ! \
    videoconvert ! x264enc bitrate=1000 tune=zerolatency ! video/x-h264 ! h264parse ! \
    video/x-h264 ! queue ! flvmux name=mux ! \
    rtmpsink location=$RTMP_DEST audiotestsrc is-live=true ! \
    audioconvert ! audioresample ! audio/x-raw,rate=48000 ! \
    voaacenc bitrate=96000 ! audio/mpeg ! aacparse ! audio/mpeg, mpegversion=4 ! mux.
```

### 拉流

#### 拉视频流

```sh

export RTMP_SRC="rtmp://192.168.104.199:1935/live/testaudio"
gst-launch-1.0 playbin uri=$RTMP_SRC

gst-launch-1.0 uridecodebin uri=$RTMP_SRC ! autovideosink

gst-launch-1.0 rtmpsrc location=$RTMP_SRC ! decodebin ! autovideosink

```

#### 拉音频流


```sh
gst-launch-1.0 rtmpsrc name=rtmpsrc location=$RTMP_SRC ! decodebin ! \
    queue ! audioconvert ! autoaudiosink

# We can vget flvdemux to pull out the audio:

gst-launch-1.0 rtmpsrc location=$RTMP_SRC ! \
    flvdemux name=t  t.audio ! decodebin ! autoaudiosink

# Incidentally, all of these work with a direct flv file:

gst-launch-1.0 filesrc location="/path/to/test.flv" ! \
    flvdemux name=t  t.audio ! decodebin ! autoaudiosink


```
## 参考
https://ossrs.net/lts/zh-cn/docs/v5/doc/low-latency