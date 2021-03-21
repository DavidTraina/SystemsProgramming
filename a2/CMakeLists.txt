cmake_minimum_required(VERSION 3.13)
project(a2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(a2
        dictionary.txt
        family.c
        family.h
        Makefile
        reading.c
        reading.h
        wheel.c)
