add_subdirectory(cooling)
add_subdirectory(sat)
add_subdirectory(satCooling)
add_subdirectory(rng)

target_link_libraries(
        cooling
        sat
)


add_executable(main main.cpp)

include(FetchContent)
FetchContent_Declare(
        cli11_proj
        QUIET
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.3.2
)

FetchContent_MakeAvailable(cli11_proj)

# And now you can use it
target_link_libraries(main PRIVATE CLI11::CLI11)