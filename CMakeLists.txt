cmake_minimum_required(VERSION 4.0)
project(GraphsLab C)

set(CMAKE_C_STANDARD 23)

add_executable(GraphsLab
        main.c
        graph.c
        set.c
        forward_list.c)
