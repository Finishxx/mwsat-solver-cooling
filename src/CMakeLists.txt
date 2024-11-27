add_subdirectory(cooling)
add_subdirectory(sat)
add_subdirectory(satCooling)

target_link_libraries(
        cooling
        sat
)