FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

#RUN sed -i 's/ports.ubuntu.com/mirrors.ustc.edu.cn/g' /etc/apt/sources.list

# Install package dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    software-properties-common \
    autoconf \
    automake \
    cmake \
    libtool \
    pkg-config \
    ca-certificates \
    libssl-dev \
    wget \
    git \
    curl \
    language-pack-en \
    locales \
    locales-all \
    vim \
    gdb \
    valgrind \
    libgtest-dev \
    libcurl4 \
    libev-dev \
    libgd-dev \
	libgstreamer1.0-dev \
    libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-bad1.0-dev \
    gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good \
    gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly \
    gstreamer1.0-libav \
    gstreamer1.0-tools \
    gstreamer1.0-x \
    gstreamer1.0-alsa \
    gstreamer1.0-gl \
    gstreamer1.0-gtk3 \
    gstreamer1.0-qt5 \
    gstreamer1.0-pulseaudio && \
    apt-get clean

# System locale
# Important for UTF-8
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8

