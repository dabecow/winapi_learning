cmake_minimum_required(VERSION 3.17)
project(winapi_practice_5 C)

set(CMAKE_C_STANDARD 99)

add_executable(winapi_practice_5 main.c drawer.h drawer.c FigureList.c FigureList.h)

add_link_options(-lgdi32)
add_link_options(-lmsimg32)
add_link_options(-lcomdlg32)