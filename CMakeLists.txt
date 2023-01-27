cmake_minimum_required(VERSION 3.20)
project(CLox C)

set(CMAKE_C_STANDARD 11)

set(SOURCES main.c common.h chunk.c memory.c memory.h chunk.h debug.h debug.c value.h value.c vm.h vm.c compiler.c compiler.h scanner.h scanner.c)

add_executable(CLox ${SOURCES})
