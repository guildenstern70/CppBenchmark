cmake_minimum_required(VERSION 3.20)
project(CppBenchmark CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
    add_compile_options(/O2 /W4 /permissive-)
else()
    add_compile_options(-O3 -Wall -Wextra -pthread)
endif()

add_executable(CppBenchmark main.cpp)
find_package(Threads REQUIRED)
target_link_libraries(CppBenchmark PRIVATE Threads::Threads)
