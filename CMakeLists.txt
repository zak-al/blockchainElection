cmake_minimum_required(VERSION 3.21)
project(projetSTR C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(projetSTR
        blockchain.c
        blockchain.h
        hashset.c
        hashset.h
        hashsetKeyToInt.c
        hashsetKeyToInt.h
        HashTable.c
        HashTable.h
        partie3_tests.c
        primalite.c
        primalite.h
        Protected.c
        Protected.h
        rsa.c
        rsa.h
        Signature.c
        Signature.h
        tests.c
        voting.c
        voting.h Arborescente.c Arborescente.h)
