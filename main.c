#include <stdio.h>
#include <stdint.h>
#include "bus.h"
#include "test.h"
#include "cpu.h"



int main(void) {
    CPU cpu = {0};
    BUS BUS = {0};

    test_cpu();
    //test_ppu_render_bg();
    test_ppu_registers();
    test_ppu_update();
    test_ppu_lyc();
    test_ppu_stat_interrupt();
    test_ppu_render_scanlines();
    test_ppu_HBlank_transition();
    test_ppu_scanline_crossing();

    return 0;
}
