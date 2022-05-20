cmake_minimum_required(VERSION 3.10)
project(PrakBS22 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(PrakBS22
        keyValueStore.c
        keyValueStore.h
        main.c
        server.c
        server.h)
