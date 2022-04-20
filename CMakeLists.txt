cmake_minimum_required(VERSION 3.21)
project(projetSTR C)

set(CMAKE_C_STANDARD 11)

include_directories(/usr/local/opt/openssl@3/include)

add_executable(projetSTR
        blockchain.c
        blockchain.h
        DEPRECATED_hashset.c
        DEPRECATED_hashset.h
        DEPRECATED_hashsetKeyToInt.c
        DEPRECATED_hashsetKeyToInt.h
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
        voting.h Arborescence.c Arborescence.h)
