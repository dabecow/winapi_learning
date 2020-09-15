cmake_minimum_required(VERSION 3.17)
project(winapi_practice_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(winapi_practice_2 main.c)

add_link_options(-lgdi32)