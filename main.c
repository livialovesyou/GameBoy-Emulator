#include <stdio.h>
#include <stdint.h>
#include "bus.h"
#include "test.h"
#include "cpu.h"
#include "ppu.h"
#include "bmp.h"
#include "timer.h"
#include "display.h"
#include "rom.h"

#define GB_FRAME_TIME_MS 16.74

int main(void) {

    DISPLAY display = {0};

    if (!display_init(&display))
    {
        return 1;
    }

    //Initialize CPU, BUS and PPU
    CPU cpu = {0};
    BUS BUS = {0};
    TIMER timer;
    PPU ppu;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    // Create a simple test pattern.
    // for (int y = 0; y < SCREEN_HEIGHT; y++) {
    //     for (int x = 0; x < SCREEN_WIDTH; x++) {
    //         framebuffer[y][x] = (x / 40) % 4;
    //     }
    // }



    ppu_init(&ppu);
    timer_init(&timer);

    BUS.ppu = &ppu;
    BUS.timer = &timer;

    if (!rom_load("tetris.gb", BUS.rom))
    {
        return 1;
    }

    ppu.lcdc = 0x91; // LCD enabled, background enabled, unsigned tile data
    ppu.bgp = 0xE4;  // Standard DMG grayscale palette
    ppu.scx = 0;
    ppu.scy = 0;
    ppu.wx = 167;    // Window outside the visible screen
    ppu.wy = 0;
    // // Tile 0: alternating black and white vertical stripes.
    // for (int row = 0; row < 8; row++)
    // {
    //     ppu.vram[row * 2] = 0xAA;
    //     ppu.vram[row * 2 + 1] = 0xAA;
    // }
    //
    // // Fill the background tile map with tile 0.
    // for (int i = 0; i < 32 * 32; i++)
    // {
    //     ppu.vram[0x1800 + i] = 0;
    // }


    //timings
    const double target_frame_time = 1.0 / 59.7;
    uint32_t fps_frame_count = 0;
    uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t frame_start = SDL_GetPerformanceCounter();
    uint64_t fps_timer = SDL_GetPerformanceCounter();

    //MAIN LOOP
    int running = 1;
    while (running) {
        //Update PPU after every instruction
        // uint64_t old_cycles = cpu.cycles;
        //
        // cpu_step(&cpu, &BUS);
        //
        // uint64_t elapsed = cpu.cycles - old_cycles;
        uint64_t elapsed = 4;

        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);

        display_process_events(&display, &running);
        if (ppu.frame_ready)
        {
            ppu.frame_ready = 0;

            display_render(&display, framebuffer);


            fps_frame_count++;
            uint64_t now = SDL_GetPerformanceCounter();
            double elapsed_seconds1 = (double)(now - fps_timer) / frequency;
            if (elapsed_seconds1 >= 1.0)
            {
                printf("FPS: %.2f\n", fps_frame_count / elapsed_seconds1);

                fps_frame_count = 0;
                fps_timer = now;
            }



            uint64_t frame_end = SDL_GetPerformanceCounter();

            double elapsed_seconds =
                (double)(frame_end - frame_start) / frequency;

            if (elapsed_seconds < target_frame_time)
            {
                double remaining_seconds = target_frame_time - elapsed_seconds;

                SDL_Delay((Uint32)(remaining_seconds * 1000.0));
            }

            frame_start = SDL_GetPerformanceCounter();
        }
    }


    //bmp_save("test.bmp", framebuffer);

    // test_cpu();
    // test_ppu();
    // test_timer();

    display_destroy(&display);
    return 0;
}
