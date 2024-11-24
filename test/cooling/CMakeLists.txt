add_executable(
        cooling_test
        cooling_test.cpp
)
target_link_libraries(
        cooling_test
        cooling
        GTest::gtest_main
)

gtest_discover_tests(cooling_test)