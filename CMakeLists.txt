cmake_minimum_required(VERSION 3.11)
project(gstreamer_cookbook)

set(CMAKE_CXX_STANDARD 20)

#include(FindPkgConfig)# equals `pkg-config --cflags --libs gstreamer-1.0`
#pkg_check_modules(GST   REQUIRED gstreamer-1.0)
#pkg_check_modules(GSTAPP REQUIRED gstreamer-app-1.0)
find_package(PkgConfig REQUIRED)


if (APPLE)
    set(GST_INCLUDE_DIRS /Library/Frameworks/GStreamer.framework/Headers)
    set(GST_LIBRARY_DIRS /Library/Frameworks/GStreamer.framework/Versions/Current/lib)
    
    include_directories(${GST_INCLUDE_DIRS})
    link_directories(${GST_LIBRARY_DIRS})
elseif(UNIX)
    pkg_search_module(GST REQUIRED gstreamer-1.0)
    include_directories(${GST_INCLUDE_DIRS})
    link_directories(${GST_LIBRARY_DIRS})
endif()

set(GST_LIBRARIES -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0)
set(GST_RTC_LIBS -lgstwebrtc-1.0 -lgstsdp-1.0 -lgstrtp-1.0 -lsoup-2.4 -ljson-glib-1.0)

add_subdirectory(util)
add_subdirectory(example)
add_subdirectory(tutorial)
add_subdirectory(test)
