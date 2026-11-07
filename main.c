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
    CPU cpu;
    BUS BUS;
    TIMER timer;
    PPU ppu;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};
    // Create a simple test pattern.
    // for (int y = 0; y < SCREEN_HEIGHT; y++) {
    //     for (int x = 0; x < SCREEN_WIDTH; x++) {
    //         framebuffer[y][x] = (x / 40) % 4;
    //     }
    // }


    cpu_init(&cpu);
    ppu_init(&ppu);
    timer_init(&timer);
    bus_init(&BUS, &ppu, &timer);

    if (!rom_load("CATRAP.gb", BUS.rom))
    {
        return 1;
    }
    printf("Cartridge type: %02X\n", BUS.rom[0x147]);
    printf("ROM size: %02X\n", BUS.rom[0x148]);

    ppu.lcdc = 0x91;
    ppu.stat = 0x80;
    ppu.scy = 0x00;
    ppu.scx = 0x00;
    ppu.ly  = 0x00;
    ppu.lyc = 0x00;
    ppu.bgp = 0xFC;
    ppu.obp0 = 0xFF;
    ppu.obp1 = 0xFF;
    ppu.wy = 0x00;
    ppu.wx = 0x00;
    BUS.interrupt_enable = 0x00;
    BUS.interrupt_flags = 0x01;
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



    printf("ROM entry: %02X %02X %02X %02X\n",
       BUS.rom[0x100],
       BUS.rom[0x101],
       BUS.rom[0x102],
       BUS.rom[0x103]);

    //MAIN LOOP
    int running = 1;
    while (running) {


        //Update PPU after every instruction
         uint64_t old_cycles = cpu.cycles;
         cpu_step(&cpu, &BUS);

         uint64_t elapsed = cpu.cycles - old_cycles;
        //printf("PC: %04X\n", cpu.pc);


        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);

        display_process_events(&display, &running, &BUS);
        if (ppu.frame_ready)
        {
            ppu.frame_ready = 0;

            display_render(&display, framebuffer);


            fps_frame_count++;
            uint64_t now = SDL_GetPerformanceCounter();
            double elapsed_seconds1 = (double)(now - fps_timer) / frequency;
            if (elapsed_seconds1 >= 1.0)
            {
                //printf("FPS: %.2f\n", fps_frame_count / elapsed_seconds1);

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

    display_destroy(&display);

    test_cpu();
    return 0;
}
