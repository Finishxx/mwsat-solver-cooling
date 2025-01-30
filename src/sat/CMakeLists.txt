add_library(sat SatCooling.cpp SatCooling.h SatConfig.h SatConfig.cpp WSatInstance.cpp WSatInstance.h SatCriteria.cpp SatCriteria.h)
target_include_directories(sat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sat rng)


