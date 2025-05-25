cmake_minimum_required(VERSION 3.27)
project(image_processing_1 C)

set(CMAKE_C_STANDARD 11)

add_executable(image_processing_1 main.c
        bmp8.h
        bmp24.h
        utils.h
        histogram.h
        utils.c
        bmp24.c
        bmp8.c
        histogram.c)
