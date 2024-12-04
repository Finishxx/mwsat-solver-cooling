add_library(rng xoshiro256plus.c)
target_include_directories(cooling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
