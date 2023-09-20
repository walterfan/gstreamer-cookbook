cmake_minimum_required(VERSION 3.11)
project(gstreamer_cookbook)

set(CMAKE_CXX_STANDARD 20)

#include(FindPkgConfig)# equals `pkg-config --cflags --libs gstreamer-1.0`
#pkg_check_modules(GST   REQUIRED gstreamer-1.0)
#pkg_check_modules(GSTAPP REQUIRED gstreamer-app-1.0)
find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET gstreamer-1.0 glib-2.0)

pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

if (APPLE)
    set(GST_INCLUDE_DIRS /Library/Frameworks/GStreamer.framework/Headers)
    set(GST_LIBRARY_DIRS /Library/Frameworks/GStreamer.framework/Versions/Current/lib)
    set(GST_LIB -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0)
elseif(UNIX)
    pkg_search_module(GST REQUIRED gstreamer-1.0)
    include_directories(${GST_INCLUDE_DIRS})
    link_directories(${GST_LIBRARY_DIRS})
endif()


add_subdirectory(tutorial)
add_subdirectory(example)