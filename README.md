# GStreamer Cookbook

A cookbook for gstreamer developer - [GStreamer cookbook](https://walterfan.github.io/gstreamer-cookbook/)


## 新手 Primer
* [GStreamer 快速入门](doc/source/1.basic/basic.md)
* [GStreamer 基本概念](doc/source/1.basic/concept.md)
* [GStreamer 设计原则](doc/source/1.basic/design.md)
* [GStreamer Element](doc/source/1.basic/gst-element.md)
* [GStreamer Bin](doc/source/1.basic/gst-bin.md)
* [GStreamer Bus](doc/source/1.basic/gst-bus.md)
* [GStreamer Pad](doc/source/1.basic/gst-pad.md)
* [GStreamer Caps](doc/source/1.basic/gst-caps.md)
* [GStreamer Buffer]
* [GStreamer Event]
* [GStreamer State](doc/source/1.basic/gst-state.md)
* [GStreamer pipeline]

## 进阶者 Advanced User

* [GStreamer plugin - boilerplate](doc/source/4.plugin/1.plugin-boilerplate.md)
* [GStreamer plugin - pads](doc/source/4.plugin/2.plugin-pads.md)
* [GStreamer plugin - chain function]
* [GStreamer plugin - event function]
* [GStreamer plugin - query function]
* [GStreamer plugin - signal](doc/source/4.plugin/7.plugin-signal.md)
* [GStreamer plugins]
* [GStreamer tools]

* [GStreamer Probe]
* [GStreamer dynamic pads]
* [GStreamer scheduling modes]
* [GStreamer Caps Negotiation]
* [GStreamer Interfaces]
* [GStreamer metadata and stream info]

## 老兵 Veteran

> 老兵永远不死，只会慢慢凋零 Old soldiers never die, they just fade away

* [GStreamer memory allocation]
* [GStreamer media types and properties]
* [GStreamer event seeking]
* [GStreamer clocking]
* [GStreamer QoS]
* [GStreamer Dynamic parameters]

* [DeepStream get started](doc/source/3.application/deepstream.md)


## Quick test

```

gst-launch-1.0 -v -e filesrc location=material/talk.mp4 \
! decodebin ! videoconvert ! autovideosink

```

## Environment

You can build a docker image by `build-docker.sh`
and start it by `start-docker.sh`

or run the below command to intall gstreamer on ubuntu

```
sudo apt-get install -y gstreamer1.0-tools \
libgstreamer1.0-dev \
libglib2.0-dev \
gstreamer1.0-nice \
gstreamer1.0-plugins-bad \
gstreamer1.0-plugins-ugly \
gstreamer1.0-plugins-good \
libgstreamer-plugins-bad1.0-dev \
gstreamer1.0-plugins-base-apps

```

## Example
* start SRS by docker
```
export CANDIDATE="192.168.0.106"
sudo docker run --rm --env CANDIDATE=$CANDIDATE \
  -p 1935:1935 -p 1975:8080 -p 1985:1985 -p 1995:8000/udp \
  registry.cn-hangzhou.aliyuncs.com/ossrs/srs:5 \
  objs/srs -c conf/rtmp2rtc.conf
```

* push local video stream from mp4 to rtmp

```
gst-launch-1.0 -vv filesrc location=material/talk.mp4 \
! decodebin \
! videoconvert ! identity drop-allocation=1 \
! x264enc tune=zerolatency ! flvmux streamable=true \
! rtmpsink location='rtmp://192.168.0.106:1935/live/waltertest'
```

* or run the C++ program

```
./build-with-vcpkg.sh
./bin/gst-pipeline-verify -f ./example/etc/pipeline.yaml -p pipeline_test_rtmp
```

* send/receive video stream over udp

```
# send video

gst-launch-1.0 -v v4l2src device=/dev/video1 ! decodebin \
! videoconvert ! omxh264enc ! video/x-h264,stream-format=byte-stream \
! rtph264pay ! udpsink host=192.168.104.214 port=5000

# receive video

gst-launch-1.0 -v udpsrc  port=5000 caps=application/x-rtp \
! rtph264depay ! avdec_h264 ! autovideosink

```
## build source code
### use vcpkg

* if you have not install vcpkg, please install it first

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install spdlog
./vcpkg install fmt
./vcpkg install yaml-cpp
```

* build with dependencies by vcpkg

```
./build-with-vcpkg.sh
```

### use conan

* if you have not install conan, please install it first
```
python3 -m venv venv
source venv/bin/activate
pip install conan
conan profile detect --force
```

* create or update [conanfile.txt](conanfile.txt) for dependencies
  
* then run the following script to build
  
```
./build-with-conan.sh

```


