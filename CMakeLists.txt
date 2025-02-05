cmake_minimum_required(VERSION 3.19)
project(sat_cooling)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")