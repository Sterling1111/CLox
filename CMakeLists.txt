cmake_minimum_required(VERSION 3.20)
project(CLox C)

set(CMAKE_C_STANDARD 11)

set(SOURCES main.c common.h chunk.c memory.c memory.h chunk.h)

add_executable(CLox ${SOURCES})
