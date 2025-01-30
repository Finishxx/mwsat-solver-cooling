add_library(dimacs_parsing dimacsParsing.cpp dimacsParsing.h)
target_include_directories(dimacs_parsing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(dimacs_parsing PUBLIC fmt::fmt)