enable_testing()
find_package(GTest REQUIRED)

add_executable(waltertest
    waltertest.cpp
)

add_executable(unit_test
    unit_test.cpp
	gobject_test.cpp
)
target_include_directories(waltertest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(unit_test GTest::gtest ${GST_LIBRARIES})
target_link_libraries(waltertest common_utils ${GST_LIBRARIES})

add_test(AllTestsInFoo unit_test)




