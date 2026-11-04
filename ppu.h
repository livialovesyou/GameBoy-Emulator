//
// Created by Livia on 04.11.2026.
//

#pragma once

#include <stdint.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define VRAM_SIZE 0x2000
#define OAM_SIZE 0xA0
#define REG_LCDC 0xFF40
#define REG_BGP  0xFF47

#define PPU_MODE_HBLANK  0
#define PPU_MODE_VBLANK  1
#define PPU_MODE_OAM     2
#define PPU_MODE_DRAWING 3

#define STAT_INT_HBLANK  0x08
#define STAT_INT_VBLANK  0x10
#define STAT_INT_OAM     0x20
#define STAT_INT_LYC     0x40
#define INT_STAT 0x02

#define INT_VBLANK 0x01

typedef struct BUS BUS;

typedef struct PPU {
    uint8_t vram[VRAM_SIZE]; // VRAM
    uint8_t oam[OAM_SIZE]; //Object Attribute Matrix

    //LCD Registers
    uint8_t lcdc; //LCD Control
    uint8_t stat;
    uint8_t scy; // Background vertical scroll
    uint8_t scx; // Background horizontal scroll
    uint8_t bgp;    // Background palette
    uint8_t ly;    // FF44
    uint8_t lyc;   // FF45
    uint8_t wy;    // FF4A
    uint8_t wx;    // FF4B

    uint8_t window_line;

    uint16_t dot;
    uint8_t mode;
    uint8_t previous_mode;

} PPU;

void ppu_init(PPU *ppu);
void ppu_decode_tile(PPU *ppu, uint16_t tile_address, uint8_t pixels[8][8]);
void ppu_render_background(PPU *ppu, uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH]);
uint8_t ppu_get_bg_color(PPU *ppu, uint8_t color_index);
uint8_t ppu_read_register(PPU *ppu, uint16_t address);
void ppu_write_register(PPU *ppu, uint16_t address, uint8_t value);
void ppu_update(PPU *ppu, BUS *BUS, uint64_t cycles, uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH]);
void ppu_update_stat(PPU *ppu);
void ppu_request_stat_interrupt(PPU *ppu, BUS *BUS);
void ppu_render_scanline(PPU *ppu, uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH]);
int ppu_window_is_visible(PPU *ppu, int x, int y);
void ppu_get_window_coordinates(PPU *ppu, int x, uint8_t *tile_x, uint8_t *tile_y, uint8_t *pixel_x, uint8_t *pixel_y);
uint8_t ppu_get_window_pixel(PPU *ppu, int x);