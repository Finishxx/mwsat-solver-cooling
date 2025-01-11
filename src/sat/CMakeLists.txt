add_library(dimacs_parsing dimacsParsing.cpp dimacsParsing.h)

target_link_libraries(dimacs_parsing PRIVATE fmt::fmt)

add_library(max_weighted_sat_solver
        MaxWSatSolver.cpp
        MaxWSatSolver.h
        MaxWSatInstance.cpp
        MaxWSatInstance.h
        SatConfig.h
        SatConfig.cpp
        Sat.cpp
        EvaluatedWSatConfig.cpp
        EvaluatedWSatConfig.h)

target_include_directories(max_weighted_sat_solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
