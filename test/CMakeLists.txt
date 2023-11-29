enable_testing()
find_package(GTest REQUIRED)

add_executable(waltertest
    waltertest.cpp
)

add_executable(unit_test
    unit_test.cpp
    hls_util_test.cpp
    playlist_util_test.cpp
    string_util_test.cpp
	gobject_test.cpp
    yaml_test.cpp
)
target_include_directories(waltertest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unit_test common_utils 
    GTest::gtest 
    yaml-cpp
    ${GST_LIBRARIES})
target_link_libraries(waltertest common_utils ${GST_LIBRARIES})

add_test(AllTestsInFoo unit_test)




