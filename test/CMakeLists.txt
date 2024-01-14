find_package(GTest REQUIRED)
if(NOT GTEST_LIBRARY)
    message("GTest not found!")
endif()

enable_testing()
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(waltertest
    waltertest.cpp
)

add_executable(unit_test
    unit_test.cpp
    hls_util_test.cpp
    playlist_util_test.cpp
    string_util_test.cpp
    time_util_test.cpp
	gobject_test.cpp
    yaml_test.cpp
)

target_include_directories(waltertest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unit_test common_utils 
    gtest
    yaml-cpp
    ${GST_LIBRARIES})
target_link_libraries(waltertest common_utils ${GST_LIBRARIES})

add_test(AllTestsInFoo unit_test)




