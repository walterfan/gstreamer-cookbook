# Cross compile

## ubuntu x86 server


### 1.准备GCC工具链 ([linaro tool chain](http://releases.linaro.org/components/toolchain/binaries/))

* GCC版本：7.3.1
* Binutils版本：2.28.2.20170706
* Glibc版本：2.25

```
sudo apt install build-essential
``


* Download tool chain (gcc 7.5.0)

[linaro tool chain](http://releases.linaro.org/components/toolchain/binaries/latest-7/aarch64-linux-gnu/)

```
wget gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu.tar.xz
```

* 下载 Jetson SDK Manager

### 2. mount ARM server by NFS

```bash
$ sudo apt update
$ sudo apt install nfs-common
$ sudo mkdir –p /mnt/rootfs
$ sudo /etc/fstab
x.x.x.x:/ /mnt/rootfs nfs defaults 0 0
$ sudo mount –t nfs 192.168.10.10:/ /mnt/rootfs
$ ls /mnt/rootfs/
```


### 3. 安装NVIDIA Jetson SDK Manager,方便下载内核源码

### 4. 下载Jetson平台对应的LT4 源码

### 5. 设置环境变量

```

export CROSS_ROOT=/mnt/rootfs

export CROSS_COMPILE=${HOME}/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-

export CC=${CROSS_COMPILE}gcc

export CXX=${CROSS_COMPILE}g++

export LD=${CROSS_COMPILE}ld

export AR=${CROSS_COMPILE}ar

export AS=${CROSS_COMPILE}as

export RANLIB=${CROSS_COMPILE}ranlib

export NVCC=${CROSS_ROOT}/usr/local/cuda/bin/nvcc

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SYSROOT/usr/lib/aarch64-linux-gnu:$SYSROOT/lib/aarch64-linux-gnu:$SYSROOT/lib

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${HOME}/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/aarch64-linux-gnu/lib64:${HOME}gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/lib
```

## Ubuntu Arm/Aarch64 平台下

1. install NFS service and export NFS directory

```bash
$ sudo apt update
$ sudo apt install nfs-kernel-server
$ sudo vim /etc/exports
/ *(rw,sync,no_subtree_check, no_root_squash)
$ sudo exportfs -a
$ sudo systemctl restart nfs-kernel-server.service
$ sudo exportfs
```


## tools

有几种工具可以帮助你确定一个库或可执行文件是为哪个平台编译的。以下是一些常见的方法：

1. **file命令**：
   在Linux系统中，`file`命令可以显示文件的类型信息，包括目标架构。例如：
   ```bash
   file libmylib.so
   ```
   输出可能会类似如下所示：
   ```
   libmylib.so: ELF 32-bit LSB shared object, ARM, EABI5 version 1 (SYSV), dynamically linked, for GNU/Linux 2.6.32, BuildID[sha1]=0x9f7a97e08c27367b73a496489071384471e4d93e, stripped
   ```
   这里显示了这是一个32位ARM架构的共享对象。

2. **readelf命令**：
   `readelf`是一个更强大的工具，可以读取和解析ELF（Executable and Linkable Format）格式文件的信息。使用`-h`选项可以查看头部信息，这通常包括了架构信息：
   ```bash
   readelf -h libmylib.so
   ```
   输出的一部分可能看起来像这样：
   ```
   Class:                             ELF32
   Data:                              2's complement, little endian
   Version:                           1 (current)
   OS/ABI:                            UNIX - System V
   ABI Version:                       0
   Type:                              DYN (Shared object file)
   Machine:                           ARM
   ...
   ```

3. **objdump命令**：
   `objdump`命令也可以用来查看文件的详细信息，包括架构：
   ```bash
   objdump -f libmylib.so
   ```
   输出的一部分可能显示如下：
   ```
   libmylib.so:     file format elf32-littlearm
   ```

通过这些命令，你可以轻松地确定库或可执行文件是否为ARM架构或其他架构编译的。如果你是在Windows环境下，可能需要使用类似Dependency Walker这样的工具来查看PE（Portable Executable）文件的架构信息。不过，大多数这类工具主要适用于Unix-like系统。


## Reference
* https://docs.nvidia.com/jetson/l4t-multimedia/mmapi_build.html
* https://docs.nvidia.com/jetson/l4t-multimedia/cross_platform_support.html
* https://developer.nvidia.com/embedded/jetson-linux
* https://docs.nvidia.com/jetson/archives/r34.1/DeveloperGuide/text/AT/JetsonLinuxToolchain.html
* https://docs.nvidia.com/jetson/archives/r34.1/DeveloperGuide/index.html