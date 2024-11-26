add_library(sat_cooling satCooling.cpp)
target_include_directories(cooling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
        sat_cooling
        sat
        cooling
)