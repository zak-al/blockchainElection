cmake_minimum_required(VERSION 3.21)
project(projetSTR C)

set(CMAKE_C_STANDARD 99)

add_executable(projetSTR main.c primalite.c primalite.h rsa.c rsa.h voting.c voting.h)
