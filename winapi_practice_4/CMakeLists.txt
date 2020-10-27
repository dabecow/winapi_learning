cmake_minimum_required(VERSION 3.17)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled main.c library/library.c FigureList.c FigureList.h)

add_link_options(-lgdi32)