# GStreamer Cookbook

A cookbook for gstreamer developer - [GStreamer cookbook](https://walterfan.github.io/gstreamer-cookbook/)

# Quick test

```

gst-launch-1.0 -v filesrc location=material/talk.mp4 ! decodebin ! videoconvert ! autovideosink

```

## build

### Environment

You can build a docker image by `build-docker.sh`
and start it by `start-docker.sh`

or run the below command to intall gstreamer on ubuntu

```
sudo apt-get install -y gstreamer1.0-tools gstreamer1.0-nice gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-plugins-good libgstreamer1.0-dev git libglib2.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base-apps

```

### use vcpkg

```

git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install spdlog
./vcpkg install fmt

export VCPKG_HOME=~/workspace/cpp/vcpkg 
mkdir -p build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_HOME/scripts/buildsystems/vcpkg.cmake
make
```

### use conan

```
mkdir -p build && \
conan install . --output-folder=build --build=missing && \
cd build && \
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake && \
make

conan install . 
cmake .. -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake
cmake --build . 
```


