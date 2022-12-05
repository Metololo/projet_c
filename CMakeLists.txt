cmake_minimum_required(VERSION 3.20)
project(projet_c C)

set(CMAKE_C_STANDARD 99)

add_executable(projet_c main.c audio_func.h audio_func.c)
