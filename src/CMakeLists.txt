add_subdirectory(cooling)
add_subdirectory(sat)

add_executable(sat_cooling main.cpp)
target_link_libraries(
        cooling
        sat
)