add_executable(basic-tutorial-1 basic-tutorial-1.c)
add_executable(basic-tutorial-2 basic-tutorial-2.c)

target_link_libraries(basic-tutorial-1 ${GST_LIBRARIES} )
target_link_libraries(basic-tutorial-2 ${GST_LIBRARIES} ${GLIB_LIBRARIES} )