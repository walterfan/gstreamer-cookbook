add_executable(gst-data-probe gst-data-probe.cpp)
add_executable(gst-queue-exam gst-queue-exam.cpp)
add_executable(gst-pipeline-verify 
    pipeline_builder.cpp 
    gst-pipeline-verify.cpp
)

add_executable(gst-fakesink gst-fakesink.cpp)

add_executable(glib-example-1 glib-example-1.cpp)
add_executable(hls-exam hls-exam.cpp pipeline_controller.cpp)
add_executable(hls-test hls-test.cpp)
add_executable(webrtc-sendrecv webrtc-sendrecv.c)

target_link_libraries(gst-data-probe 
    ${GST_LIBRARIES} 
    common_utils 
    fmt::fmt 
    spdlog::spdlog
)

target_link_libraries(gst-pipeline-verify
    ${GST_LIBRARIES} 
    common_utils 
    fmt::fmt 
    spdlog::spdlog
)


target_link_libraries(gst-queue-exam ${GST_LIBRARIES})
target_link_libraries(glib-example-1 ${GST_LIBRARIES})
target_link_libraries(hls-test ${GST_LIBRARIES} fmt::fmt)
target_link_libraries(hls-exam common_utils ${GST_LIBRARIES} fmt::fmt)
target_link_libraries(gst-fakesink ${GST_LIBRARIES})

target_include_directories(webrtc-sendrecv PRIVATE 
    /usr/include/gstreamer-1.0
    /usr/include/libsoup-2.4 
    /usr/include/json-glib-1.0)
target_link_libraries(webrtc-sendrecv ${GST_LIBRARIES} ${GST_RTC_LIBS})