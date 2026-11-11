#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bus.h"
#include "test.h"
#include "cpu.h"
#include "ppu.h"
#include "bmp.h"
#include "timer.h"
#include "display.h"
#include "rom.h"
#include "save.h"

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

    cpu_init(&cpu);
    ppu_init(&ppu);
    timer_init(&timer);
    bus_init(&BUS, &ppu, &timer);

    uint8_t *rom_data = NULL;
    size_t rom_size = 0;

    printf("Internal DIV: %04X\n", timer.divider);
    printf("Bus DIV: %02X\n", bus_read_byte(&BUS, 0xFF04));

    if (!rom_load("METROID.gb", &BUS.rom, &BUS.rom_size))
    {
        display_destroy(&display);
        return 1;
    }
    ppu_boot(&ppu, &BUS);

    printf("Loaded ROM: %zu bytes\n", BUS.rom_size);
    printf("Cartridge type: %02X\n", BUS.rom[0x147]);
    printf("ROM size: %02X\n", BUS.rom[0x148]);
    printf("RAM size code: %02X\n", BUS.rom[0x149]);
    BUS.mbc_type = BUS.rom[0x0147];
    printf("MBC type: %02X\n", BUS.mbc_type);

    if (BUS.mbc_type == 0x13)
    {
        if (save_load(&BUS, "PKM_RED.sav"))
            printf("Save file loaded.\n");
        else
            printf("No existing save file found.\n");
    }

    printf("Cartridge type: %02X\n", BUS.mbc_type);

    //timings
    const double target_frame_time = 1.0 / 59.7;
    uint32_t fps_frame_count = 0;
    uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t frame_start = SDL_GetPerformanceCounter();
    uint64_t fps_timer = SDL_GetPerformanceCounter();

    uint64_t cycle_count = 0;
    uint64_t frame_cycles = 0;

    printf("ROM entry: %02X %02X %02X %02X\n",
       BUS.rom[0x100],
       BUS.rom[0x101],
       BUS.rom[0x102],
       BUS.rom[0x103]);

    //MAIN LOOP
    int running = 1;
    while (running) {

         uint64_t old_cycles = cpu.cycles;
         cpu_step(&cpu, &BUS);
        if (cpu.pc == 0x0100) {
            printf("CPU returned to cartridge entry point\n");
        }

         uint64_t elapsed = cpu.cycles - old_cycles;
        //printf("PC: %04X\n", cpu.pc);
        cycle_count += elapsed;
        frame_cycles += elapsed;

        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);


        static uint64_t debug_counter = 0;
        debug_counter++;
        if (debug_counter % 1000000 == 0) {
            printf("PC: %04X | LY: %d | IF: %02X | IE: %02X\n",
                   cpu.pc,
                   ppu.ly,
                   BUS.interrupt_flags,
                   BUS.interrupt_enable);
        }


        display_process_events(&display, &running, &BUS);
        if (ppu.frame_ready)
        {
            ppu.frame_ready = 0;

            display_render(&display, framebuffer);
            //printf("Frame cycles: %llu\n",(unsigned long long)frame_cycles);
            frame_cycles = 0;

            fps_frame_count++;

            uint64_t now = SDL_GetPerformanceCounter();
            double elapsed_seconds1 = (double)(now - fps_timer) / frequency;
            if (elapsed_seconds1 >= 1.0)
            {
                printf("FPS: %.2f\n", fps_frame_count / elapsed_seconds1);
                //printf("CPU cycles/sec: %llu\n",(unsigned long long)(cycle_count / elapsed_seconds1));
                cycle_count = 0;

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



    if (BUS.mbc_type == 0x13)
    {
        if (save_write(&BUS, "PKM_RED.sav"))
            printf("Game saved.\n");
        else
            printf("Failed to save game.\n");
    }
    free(BUS.rom);
    BUS.rom = NULL;
    free(rom_data);
    // test_cpu();
    // test_ppu();
    // test_timer();

    return 0;
}
