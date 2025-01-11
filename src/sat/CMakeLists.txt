add_library(sat Sat.cpp
        dimacsParsing.cpp
        dimacsParsing.h)
target_include_directories(sat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sat PRIVATE fmt::fmt)

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