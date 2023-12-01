# GStreamer Cookbook

A cookbook for gstreamer developer - [GStreamer cookbook](https://walterfan.github.io/gstreamer-cookbook/)

# Quick test

```

gst-launch-1.0 -v -e filesrc location=material/talk.mp4 \
! decodebin ! videoconvert ! autovideosink

```

## build

### Environment

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
  -p 1935:1935 -p 8080:8080 -p 1985:1985 -p 8000:8000/udp \
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
./build/example/gst-pipeline-verify -f ./example/etc/pipeline.yaml -p pipeline_test_rtmp
```

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

* create [conanfile.txt](conanfile.txt)
  
* then run the following script to build
  
```
./build-with-conan.sh

```


