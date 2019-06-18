cmake_minimum_required(VERSION 3.14)
project(SimpleRayTracer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

include_directories(src)
include_directories(src/lodepng)

add_executable(SimpleRayTracer
        src/lodepng/lodepng.cpp
        src/lodepng/lodepng.h
        src/camera.h
        src/main.cpp
        src/material.h
        src/math_util.h
        src/ray.h
        src/sphere.h
        src/surface.h
        src/surface_list.h
        src/vec3.h)

target_link_libraries(SimpleRayTracer Boost::program_options)
