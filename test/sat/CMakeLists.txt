add_executable(dimacs_parsing_test
        dimacsParsingTest.cpp
)
target_link_libraries(
        dimacs_parsing_test
        dimacs_parsing
        GTest::gtest_main
)

add_executable(
        sat_test
        sat_test.cpp
)
target_link_libraries(
        sat_test
        GTest::gtest_main
)


gtest_discover_tests(sat_test)