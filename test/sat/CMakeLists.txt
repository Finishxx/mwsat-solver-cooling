# Parser
add_executable(dimacs_parsing_test dimacsParsingTest.cpp)
target_link_libraries(
        dimacs_parsing_test
        dimacs_parsing
        GTest::gtest_main
)
gtest_discover_tests(dimacs_parsing_test)

# Instance
add_executable(
        instance_test
        SatInstanceTest.cpp
)
target_link_libraries(
        instance_test
        sat
        dimacs_parsing
        GTest::gtest_main
)
gtest_discover_tests(instance_test)

# Cooling
add_executable(sat_cooling_test SatCoolingTest.cpp)
target_link_libraries(
        sat_cooling_test
        sat
        dimacs_parsing
        GTest::gtest_main
)
gtest_discover_tests(sat_cooling_test)

