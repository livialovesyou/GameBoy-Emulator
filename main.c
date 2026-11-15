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

static void get_save_filename(const char *rom_filename,
                              char *save_filename,
                              size_t buffer_size)
{
    snprintf(save_filename, buffer_size, "%s", rom_filename);

    char *extension = strrchr(save_filename, '.');

    if (extension != NULL)
        strcpy(extension, ".sav");
    else
        strncat(save_filename, ".sav",
                buffer_size - strlen(save_filename) - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        printf("Usage: %s <rom_file>\n", argv[0]);
        return 1;
    }

    const char *rom_filename = argv[1];

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
    //size_t rom_size = 0;

    printf("Internal DIV: %04X\n", timer.divider);
    printf("Bus DIV: %02X\n", bus_read_byte(&BUS, 0xFF04));

    if (!rom_load(rom_filename, &BUS.rom, &BUS.rom_size))
    {
        printf("Failed to load ROM: %s\n", rom_filename);
        display_destroy(&display);
        return 1;
    }
    char save_filename[256];
    get_save_filename(rom_filename, save_filename, sizeof(save_filename));
    ppu_boot(&ppu, &BUS);

    printf("Loaded ROM: %zu bytes\n", BUS.rom_size);
    printf("Cartridge type: %02X\n", BUS.rom[0x147]);
    printf("ROM size: %02X\n", BUS.rom[0x148]);
    printf("RAM size code: %02X\n", BUS.rom[0x149]);
    BUS.mbc_type = BUS.rom[0x0147];
    printf("MBC type: %02X\n", BUS.mbc_type);

    if (BUS.mbc_type == 0x03 ||
    BUS.mbc_type == 0x06 ||
    BUS.mbc_type == 0x09 ||
    BUS.mbc_type == 0x0D ||
    BUS.mbc_type == 0x10 ||
    BUS.mbc_type == 0x13 ||
    BUS.mbc_type == 0x1B ||
    BUS.mbc_type == 0x1E)
    {
        if (save_load(&BUS, save_filename))
            printf("Save file loaded: %s\n", save_filename);
        else
            printf("No existing save file found: %s\n", save_filename);
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

    uint64_t fps_start = SDL_GetPerformanceCounter();
    uint32_t fps_frames = 0;
    const double target_fps = 59.7275;
    const uint64_t performance_frequency = SDL_GetPerformanceFrequency();
    const uint64_t frame_ticks =
        (uint64_t)((double)performance_frequency / target_fps);

    uint64_t next_frame = SDL_GetPerformanceCounter();

    // Main loop
    int running = 1;
    while (running)
    {
        uint64_t old_cycles = cpu.cycles;

        cpu_step(&cpu, &BUS);

        uint64_t elapsed = cpu.cycles - old_cycles;
        frame_cycles += elapsed;

        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);

        display_process_events(&display, &running, &BUS);

        if (ppu.frame_ready)
        {
            ppu.frame_ready = 0;
            display_render(&display, framebuffer);

            next_frame += frame_ticks;

            uint64_t now = SDL_GetPerformanceCounter();

            if (now > next_frame)
            {
                // Emulator fell behind; reset the deadline.
                next_frame = now;
            }
            else
            {
                while ((now = SDL_GetPerformanceCounter()) < next_frame)
                {
                    uint64_t remaining_ticks = next_frame - now;
                    uint64_t remaining_ns =
                        (remaining_ticks * 1000000000ULL) / performance_frequency;

                    // Sleep most of the remaining time, then wait precisely.
                    if (remaining_ns > 1000000ULL)
                        SDL_DelayNS(remaining_ns - 500000ULL);
                }
            }
        }
    }

    display_destroy(&display);



    if (BUS.mbc_type == 0x03 ||
    BUS.mbc_type == 0x06 ||
    BUS.mbc_type == 0x09 ||
    BUS.mbc_type == 0x0D ||
    BUS.mbc_type == 0x10 ||
    BUS.mbc_type == 0x13 ||
    BUS.mbc_type == 0x1B ||
    BUS.mbc_type == 0x1E)
    {
        if (save_write(&BUS, save_filename))
            printf("Game saved: %s\n", save_filename);
        else
            printf("Failed to save game: %s\n", save_filename);
    }

    free(BUS.rom);
    BUS.rom = NULL;
    free(rom_data);
    // test_cpu();
    // test_ppu();
    // test_timer();

    return 0;
}
