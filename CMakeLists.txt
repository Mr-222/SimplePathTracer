cmake_minimum_required(VERSION 3.16)
project(my_pathtracer)

set(CMAKE_CXX_STANDARD 17)

add_executable(my_pathtracer main.cpp vec3.h color.h ray.h hittable.h sphere.h hittable_list.h rtweekend.h camera.h material.h)