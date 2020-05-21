cmake_minimum_required(VERSION 3.16)
project(pc_practico1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(pc_practico1 main.c)