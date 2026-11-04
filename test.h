//
// Created by Livia on 23.10.2026.
//

#pragma once

#include "bus.h"
#include <stdint.h>


void echo_ram_test_1(void);
void test_unimplemented_address(void);
void test_cpu_registers_1(void);
void test_cpu_flags_1(void);
void test_cpu_opcodes_1(void);
void test_cpu_opcodes_2(void);
void test_cpu_opcodes_3(void);
void test_cpu_opcodes_4(void);
void test_cpu_opcodes_5(void);
void test_cpu_opcodes_6(void);
void test_cpu_opcodes_7(void);
void test_cpu_opcodes_8(void);
void test_cpu_opcodes_9(void);
void test_cpu_interrupts(void);
void test_ppu_render_bg(void);
void test_ppu_registers(void);
void test_ppu_update(void);
void test_ppu_lyc(void);
void test_ppu_stat_interrupt(void);
void test_ppu_render_scanlines(void);
void test_ppu_HBlank_transition(void);
void test_ppu_scanline_crossing(void);

void test_cpu(void);