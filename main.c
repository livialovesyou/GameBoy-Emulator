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

    if (!rom_load("PKM_RED.gb", &BUS.rom, &BUS.rom_size))
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



    uint64_t cycle_count = 0;
    uint64_t frame_cycles = 0;

    printf("ROM entry: %02X %02X %02X %02X\n",
       BUS.rom[0x100],
       BUS.rom[0x101],
       BUS.rom[0x102],
       BUS.rom[0x103]);

    // Timing
    const uint64_t frequency = SDL_GetPerformanceFrequency();
    const double target_fps = 59.7275;
    const uint64_t frame_ticks =
        (uint64_t)((double)frequency / target_fps);

    uint64_t next_frame_time = SDL_GetPerformanceCounter();

    // Main loop
    int running = 1;

    while (running)
    {
        uint64_t old_cycles = cpu.cycles;

        cpu_step(&cpu, &BUS);

        uint64_t elapsed = cpu.cycles - old_cycles;

        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);

        display_process_events(&display, &running, &BUS);

        if (ppu.frame_ready)
        {
            ppu.frame_ready = 0;
            display_render(&display, framebuffer);
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
