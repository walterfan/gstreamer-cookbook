set(common_utils_SRCS
    time_util.cpp
    hls_util.cpp
    gst_util.cpp
    string_util.cpp
    file_util.cpp
)

add_library(common_utils STATIC ${common_utils_SRCS})
target_include_directories(common_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common_utils PRIVATE 
    fmt::fmt 
    spdlog::spdlog
    yaml-cpp
)
