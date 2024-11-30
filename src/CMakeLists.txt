add_subdirectory(cooling)
add_subdirectory(sat)
add_subdirectory(satCooling)
add_subdirectory(rng)

target_link_libraries(
        cooling
        sat
)