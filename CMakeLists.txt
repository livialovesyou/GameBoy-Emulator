cmake_minimum_required(VERSION 4.3)
project(GB_Emulator C)

set(CMAKE_C_STANDARD 23)

add_executable(GB_Emulator main.c
        BUS.c
        BUS.h
        test.c
        test.h
        cpu.c
        cpu.h
        ppu.c
        ppu.h
        timer.c
        timer.h
        bmp.c
        bmp.h
        timer.c
        timer.h
)
