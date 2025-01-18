# Parser
add_executable(dimacs_parsing_test dimacsParsingTest.cpp)
target_link_libraries(
        dimacs_parsing_test
        dimacs_parsing
        GTest::gtest_main
)
gtest_discover_tests(dimacs_parsing_test)

# Solver
add_executable(
        solver_test
        MaxWSatSolverTest.cpp
        MaxWSatInstanceTest.cpp
)
target_link_libraries(
        solver_test
        max_weighted_sat_solver
        dimacs_parsing
        GTest::gtest_main
)
gtest_discover_tests(solver_test)
