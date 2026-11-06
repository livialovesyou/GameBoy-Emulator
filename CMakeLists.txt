cmake_minimum_required(VERSION 4.3)
project(GB_Emulator C)

set(CMAKE_C_STANDARD 23)

include(FetchContent)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.2.26
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

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
        display.c
        display.h
        rom.c
        rom.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
