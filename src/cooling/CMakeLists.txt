add_library(cooling Cooling.cpp Cooling.h)
target_link_libraries(cooling rng myDebug)
target_include_directories(cooling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
