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
* 推流
```
gst-launch-1.0 -v -e videotestsrc ! clockoverlay ! queue ! videoconvert ! x264enc tune=zerolatency ! flvmux streamable=true ! rtmpsink location='rtmp://192.168.0.104:1935/live/waltertest'
```

在我的 macbook 上，通过以下命令从摄像头中捕获视频流，发送到 SRS 上

```
gst-launch-1.0 -v -m avfvideosrc device-index=0 ! "video/x-raw,width=1280,height=720" ! queue ! videoconvert ! x264enc tune=zerolatency ! flvmux streamable=true ! rtmpsink location='rtmp://192.168.104.214:1935/live/macvideo'
```

note: x264enc parameters: speed-preset=superfast tune=zerolatency psy-tune=grain sync-lookahead=5 bitrate=480 key-int-max=50 ref=2 

* 拉流

```

```

## 参考
https://ossrs.net/lts/zh-cn/docs/v5/doc/low-latency