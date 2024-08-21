# Opencv

## cross compile

To compile an OpenCV program on an x86 server for an ARM platform, you'll need to use cross-compilation. Here's a general guide on how to do it:

### 1. Set Up Your Cross-Compilation Environment

#### Install Required Tools
You will need a cross-compiler for ARM. For example, you can use the `gcc-arm-linux-gnueabi` or `gcc-arm-linux-gnueabihf` toolchain.

On a Debian/Ubuntu system, you can install it using:
```sh
sudo apt-get update
sudo apt-get install gcc-arm-linux-gnueabi g++-arm-linux-gnueabi
```

For ARM64 (AArch64):
```sh
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

#### Install CMake
Make sure you have CMake installed:
```sh
sudo apt-get install cmake
```

### 2. Download and Prepare OpenCV

#### Get OpenCV Source Code
Download the OpenCV source code from the official repository or website.

```sh
git clone https://github.com/opencv/opencv.git
cd opencv
git checkout <desired-version>  # e.g., git checkout 4.5.1
```

#### Create a Build Directory
Create a directory for the build process:
```sh
mkdir build
cd build
```

### 3. Configure the Build with CMake

Use CMake to configure the build system for cross-compilation.

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=../platforms/linux/arm-gnueabi.toolchain.cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..
```

If you don’t have a predefined toolchain file, you can create one. Here’s an example for ARM:

#### Create a Toolchain File

Create a file named `arm-gnueabi.toolchain.cmake` with the following content:

```cmake
# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)
set(CMAKE_INSTALL_PREFIX /your/target/install/prefix)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  /path/to/your/target/rootfs)

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Adjust the following paths to your toolchain
set(CMAKE_SYSROOT /path/to/your/target/rootfs)
```

Modify the paths to match your setup.

### 4. Build OpenCV

Run the build process using `make`.

```sh
make -j$(nproc)
```

### 5. Install OpenCV

After the build completes, install OpenCV to the specified location.

```sh
make install
```

### 6. Cross-Compile Your OpenCV Program

Now you can cross-compile your OpenCV program.

#### Create a Simple Program

Create a simple program, e.g., `test.cpp`:

```cpp
#include <opencv2/opencv.hpp>

int main() {
    cv::Mat img = cv::imread("test.jpg", cv::IMREAD_COLOR);
    if (img.empty()) {
        return -1;
    }
    cv::imshow("Display Image", img);
    cv::waitKey(0);
    return 0;
}
```

#### Cross-Compile Your Program

Create a `CMakeLists.txt` file for your program:

```cmake
cmake_minimum_required(VERSION 3.0)
project(TestOpenCV)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(test test.cpp)
target_link_libraries(test ${OpenCV_LIBS})
```

Create a build directory for your program:

```sh
mkdir build
cd build
```

Run CMake with the toolchain file:

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=path/to/your/arm-gnueabi.toolchain.cmake ..
make
```

This should produce an executable that you can run on your ARM platform.

### 7. Transfer and Run on ARM Platform

Transfer the compiled executable and any necessary libraries to your ARM platform using `scp` or any other file transfer method.

```sh
scp test user@arm_device:/path/to/destination
scp -r /your/target/install/prefix/lib user@arm_device:/path/to/destination/lib
```

On your ARM device, you may need to set the library path:

```sh
export LD_LIBRARY_PATH=/path/to/destination/lib:$LD_LIBRARY_PATH
./test
```

This should run your OpenCV program on the ARM platform.