cmake_minimum_required(VERSION 3.27)
project(untitled C)

# Ustawienie kompilatora C
set(CMAKE_C_COMPILER gcc)

# Ustawienie standardu języka C
set(CMAKE_C_STANDARD 11)

add_executable(untitled
        Lab2/collatz.c
        Lab2/client.c
        Lab3/reverse.c
        Lab3/find.c
        Lab4/task1.c
        Lab4/task2.c
        Lab5/task1.c
        Lab5/catcher.c
        Lab5/sender.c
        Lab6/task1.c
        Lab6/task2.c
        Lab6/calculate_integration.c
        Lab7/client.c
        Lab7/server.c
        Lab8/client.c
        Lab8/printer.c
        Lab8/config.h
)
