#include <stdio.h>
#include <stdint.h>
#include "bus.h"
#include "test.h"
#include "cpu.h"
#include "ppu.h"
#include "bmp.h"
#include "timer.h"


int main(void) {

    //Initialize CPU, BUS and PPU
    CPU cpu = {0};
    BUS BUS = {0};
    TIMER timer;
    PPU ppu;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_init(&ppu);
    timer_init(&timer);

    BUS.ppu = &ppu;
    BUS.timer = &timer;

    bool running = false;

    while (running) {
        //Update PPU after every instruction
        uint64_t old_cycles = cpu.cycles;

        cpu_step(&cpu, &BUS);

        uint64_t elapsed = cpu.cycles - old_cycles;

        bus_update_dma(&BUS, elapsed);
        ppu_update(&ppu, &BUS, elapsed, framebuffer);
        timer_update(&timer, elapsed, &BUS.interrupt_flags);
    }

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            framebuffer[y][x] = (x / 40) % 4;
        }
    }

    bmp_save("test.bmp", framebuffer);

    test_cpu();
    test_ppu();
    test_timer();


    return 0;
}
