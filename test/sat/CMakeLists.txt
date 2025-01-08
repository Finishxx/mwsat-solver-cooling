add_executable(
        sat_test
        sat_test.cpp
        dimacsParsingTest.cpp
)
target_link_libraries(
        sat_test
        sat
        GTest::gtest_main
)

gtest_discover_tests(sat_test)