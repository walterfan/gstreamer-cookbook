add_executable(gst-queue-exam gst-queue-exam.cpp)
add_executable(pipeline-exam-1 pipeline-exam-1.cpp)
add_executable(glib-example-1 glib-example-1.cpp)
add_executable(hlssink2-example hlssink2-example.cpp)

target_link_libraries(gst-queue-exam ${GST_LIBRARIES})
target_link_libraries(pipeline-exam-1 ${GST_LIBRARIES})
target_link_libraries(glib-example-1 ${GST_LIBRARIES})
target_link_libraries(hlssink2-example ${GST_LIBRARIES})