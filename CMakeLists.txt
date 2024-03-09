cmake_minimum_required(VERSION 3.27)
project(untitled C)

# Ustawienie kompilatora C
set(CMAKE_C_COMPILER gcc)

# Ustawienie standardu języka C
set(CMAKE_C_STANDARD 11)

add_executable(untitled
        collatz.c
        dynamic_client.c
        client.c)
