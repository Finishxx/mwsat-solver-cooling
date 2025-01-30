add_library(rng Rng.cpp Rng.h xoshiro256plus.c xoshiro256plus.h)
target_include_directories(rng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
