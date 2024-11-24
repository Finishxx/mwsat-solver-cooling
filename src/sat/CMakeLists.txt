add_library(sat sat.cpp
        dimacsParser.cpp
        dimacsParser.h)
target_include_directories(sat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})