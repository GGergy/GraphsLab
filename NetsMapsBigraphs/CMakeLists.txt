cmake_minimum_required(VERSION 4.1)
project(NetsMapsBigraphs C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic)
include_directories(include)

set(SOURCES
        main.c
        src/graph.c
        src/set.c
        src/forward_list.c
        src/net.c
        src/map.c
        src/bipart.c
)

add_executable(NetsMapsBigraphs ${SOURCES})
