cmake_minimum_required(VERSION 3.17)
project(winapi_practice_7_1 C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(winapi_practice_7_1 main.c)
add_link_options(-lgdi32)
add_link_options(-lcomdlg32)

