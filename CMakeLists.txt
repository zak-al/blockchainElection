cmake_minimum_required(VERSION 3.21)
project(projetSTR C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(projetSTR
        hashset.c
        hashset.h
        hashsetKeyToInt.c
        hashsetKeyToInt.h
        partie3_tests.c
        primalite.c
        primalite.h
        rsa.c
        rsa.h
        tests.c
        voting.c
        voting.h HashTable.c HashTable.h)
