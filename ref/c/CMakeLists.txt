cmake_minimum_required(VERSION 3.10)
project(c C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(tests
    segwit_addr.c
    segwit_addr.h
    tests.c
    txref_code.c
    txref_code.h
    txrefext_code.c
    txrefext_code.h)

add_executable(tests-ext
    segwit_addr.c
    segwit_addr.h
    tests-ext.c
    txref_code.c
    txref_code.h
    txrefext_code.c
    txrefext_code.h)
