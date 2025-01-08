add_library(sat Sat.cpp
        dimacsParsing.cpp
        dimacsParsing.h)
target_include_directories(sat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sat PRIVATE fmt::fmt)