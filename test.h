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
void test_ppu_window_helper(void);
void test_ppu_window_coords(void);
void test_ppu_window_pixel(void);
void test_ppu_render_scanline_window(void);
void test_ppu_window_line_progression(void);
void test_ppu_window_line_reset(void);
void test_ppu_get_sprite(void);
void test_ppu_sprite_overlaps_scanline(void);
void test_ppu_get_scanline_sprites(void);
void test_ppu_get_sprite_pixel(void);
void test_ppu_sprite_8x16(void);
void test_ppu_render_sprites(void);
void test_sprite_palettes(void);
void test_sprite_priority_over_window(void);
void test_sprite_x_priority(void);
void test_sprite_oam_priority(void);
void test_sprites_render_with_background_disabled(void);
void test_sprite_behind_background(void);
void test_sprite_behind_background_color_zero(void);
void test_sprite_transparency(void);
void test_sprite_horizontal_flip(void);
void test_sprite_vertical_flip(void);
void test_sprite_8x16(void);
void test_sprite_8x16_vertical_flip(void);
void test_stat_interrupt_edge(void);
void test_ppu_memory_access(void);
void test_oam_dma(void);
void test_dma_cpu_access(void);
void test_dma_cpu_access_1(void);
void test_dma_loop_integration(void);
void test_oam_dma_restart(void);
void test_oam_dma_boundaries(void);


void test_ppu_fifo(void);
void test_ppu_fetcher(void);
void test_ppu_fetcher_multiple_tiles(void);
void test_ppu_output_pixel(void);
void test_ppu_scx_scroll(void);
void test_ppu_window_fifo(void);
void test_ppu_window_line_counter(void);
void test_ppu_window_visibility_limits(void);
void test_ppu_sprite_fifo(void);
void test_ppu_sprite_over_background(void);
void test_ppu_sprite_behind_background(void);
void test_ppu_sprite_priority(void);
void test_ppu_sprite_fetch_delay(void);
void test_ppu_sprite_mode3_duration(void);
void test_ppu_sprite_position_timing(void);
void test_ppu_window_disabled(void);


void test_timer_div(void);
void test_timer_frequency(void);
void test_timer_overflow(void);
void test_timer_tima_write_during_overflow(void);
void test_timer_tma_write_during_overflow(void);
void test_timer_div_glitch(void);
void test_timer_tac_glitch(void);
void test_timer_bus_interrupt(void);
void test_timer_registers(void);
void test_cpu_timer_integration(void);
void test_cpu_halt_timer_wakeup(void);
void test_cpu_halt_wakeup_ime_disabled(void);
void test_cpu_halt_bug_execution(void);

void test_cpu_stop_wakeup(void);



void test_cpu(void);
void test_ppu(void);
void test_timer(void);