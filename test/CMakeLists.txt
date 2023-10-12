enable_testing()
find_package(GTest REQUIRED)

add_executable(unit_test
    unit_test.cpp
	gobject_test.cpp
)

target_link_libraries(unit_test GTest::gtest ${GST_LIBRARIES})

add_test(AllTestsInFoo unit_test)




