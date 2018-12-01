cmake_minimum_required(VERSION 3.12)
project(Ukkonen)

set(CMAKE_CXX_STANDARD 14)

include_directories(src/lz77)

add_executable(Ukkonen
        src/lz77/compress.c
        src/lz77/compress.h
        src/lz77/decompress.c
        src/lz77/main.c
        src/lz77/operations.c
        src/lz77/structure.c
        src/lz77/ukkonen.h)
