add_subdirectory(cooling)
add_subdirectory(sat)
add_subdirectory(rng)
add_subdirectory(dimacs)
add_subdirectory(debug)

add_executable(main main.cpp)
target_link_libraries(main PUBLIC cooling sat dimacs_parsing rng)

# CLI11
include(FetchContent)
FetchContent_Declare(
        cli11_proj
        QUIET
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)
target_link_libraries(main PRIVATE CLI11::CLI11)


# FMT
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)
