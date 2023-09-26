add_executable(gst-queue-exam gst-queue-exam.cpp)
add_executable(basic-example-1 basic-example-1.cpp)
add_executable(glib-example-1 glib-example-1.cpp)

target_link_libraries(gst-queue-exam ${GST_LIBRARIES})
target_link_libraries(basic-example-1 ${GST_LIBRARIES})
target_link_libraries(glib-example-1 ${GST_LIBRARIES})