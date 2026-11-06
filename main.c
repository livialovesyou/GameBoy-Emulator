#include <stdio.h>
#include <stdint.h>
#include "bus.h"
#include "test.h"
#include "cpu.h"
#include "ppu.h"
#include "bmp.h"
#include "timer.h"
#include "display.h"

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
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            framebuffer[y][x] = (x / 40) % 4;
        }
    }



    ppu_init(&ppu);
    timer_init(&timer);

    BUS.ppu = &ppu;
    BUS.timer = &timer;

    int running = 1;

    while (running) {
        //Update PPU after every instruction
        uint64_t old_cycles = cpu.cycles;

        cpu_step(&cpu, &BUS);

        uint64_t elapsed = cpu.cycles - old_cycles;

        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);

        display_process_events(&display, &running);

        // if (ppu.frame_ready)
        // {
            ppu.frame_ready = 0;
            display_render(&display, framebuffer);
        //}
    }


    //bmp_save("test.bmp", framebuffer);

    // test_cpu();
    // test_ppu();
    // test_timer();

    display_destroy(&display);
    return 0;
}
