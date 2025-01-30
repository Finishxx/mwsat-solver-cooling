# Create an INTERFACE target for global includes
add_library(myDebug debug.cpp debug.h)

# Define where debug.h is located
target_include_directories(myDebug PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
