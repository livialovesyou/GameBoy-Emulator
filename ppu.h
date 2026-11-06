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

#define OAM_SPRITE_COUNT 40
#define OAM_SPRITE_SIZE 4
#define MAX_SPRITES_PER_SCANLINE 10

typedef struct BUS BUS;


typedef struct
{
    uint8_t color;
    uint8_t palette;
    uint8_t behind_bg;
    uint8_t priority;
    uint8_t x;
} SpritePixel;

typedef struct
{
    SpritePixel pixels[80];
    uint8_t head;
    uint8_t count;
} SpriteFIFO;


typedef struct {
    uint8_t color;
} Pixel;

typedef struct {
    Pixel pixels[16];
    uint8_t head;
    uint8_t count;
} PixelFIFO;

typedef struct {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t attributes;
    int oam_index;
} Sprite;


typedef struct PPU {
    uint8_t vram[VRAM_SIZE]; // VRAM
    uint8_t oam[OAM_SIZE]; //Object Attribute Matrix

    //LCD Registers
    uint8_t lcdc; //LCD Control
    uint8_t stat;
    uint8_t scy; // Background vertical scroll
    uint8_t scx; // Background horizontal scroll
    uint8_t bgp;    // Background palette
    uint8_t obp0;   //Palette 1
    uint8_t obp1;   //Palette 2
    uint8_t ly;    // FF44
    uint8_t lyc;   // FF45
    uint8_t wy;    // FF4A
    uint8_t wx;    // FF4B

    uint8_t window_line;
    uint8_t window_active;
    uint8_t window_x;
    uint8_t window_y;
    uint8_t window_fetch_delay;
    uint8_t window_discard_pixels;
    uint8_t window_used_this_line;

    uint16_t dot;
    uint8_t mode;
    uint8_t previous_mode;
    uint8_t stat_irq_line;
    uint8_t lcd_was_enabled;


    //FIFO
    PixelFIFO bg_fifo; //Holds pixels waiting to be displayed
    SpriteFIFO sprite_fifo;
    uint8_t fetcher_x;  //Current tile-map X position
    uint8_t fetcher_tile;   //Tile number being fetched
    uint8_t fetcher_low;    //Low bitplane byte
    uint8_t fetcher_high;   //High bitplane byte
    uint8_t fetcher_step;   //Current fetcher stage
    uint8_t fetcher_cycle;  //Timing within a fetcher stage
    Sprite selected_sprites[MAX_SPRITES_PER_SCANLINE];

    uint8_t sprite_count;
    uint16_t output_x;  //Current screen X coordinate
    uint8_t discard_pixels; //	Pixels discarded for SCX fine scrolling
    uint8_t sprite_fetch_active; //whether a sprite fetch is in progress.
    uint8_t sprite_fetch_delay; //dots remaining in the fetch.
    uint8_t sprite_fetch_index; //which selected sprite is being fetched.

    uint8_t frame_ready;

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

void ppu_get_sprite(PPU *ppu, int index, Sprite *sprite);
int ppu_sprite_overlaps_scanline(PPU *ppu, Sprite *sprite, int scanline);
int ppu_get_scanline_sprites(PPU *ppu, int scanline, Sprite sprites[MAX_SPRITES_PER_SCANLINE]);
uint8_t ppu_get_sprite_pixel(PPU *ppu, Sprite *sprite, int pixel_x, int pixel_y);
void ppu_render_sprites(PPU *ppu, int scanline, uint8_t pixels[SCREEN_WIDTH], uint8_t bg_color_indices[SCREEN_WIDTH]);
uint8_t ppu_get_sprite_color(PPU *ppu, Sprite *sprite, uint8_t color_index);
void ppu_sort_sprites(Sprite sprites[], int count);
int ppu_vram_accessible(PPU *ppu);
int ppu_oam_accessible(PPU *ppu);

void ppu_fifo_reset(PixelFIFO *fifo);
void ppu_fifo_push(PixelFIFO *fifo, uint8_t color);
int ppu_fifo_pop(PixelFIFO *fifo, uint8_t *color);
void ppu_fetcher_update(PPU *ppu);
void ppu_output_pixel(PPU *ppu, uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH]);
void ppu_start_drawing(PPU *ppu);
void ppu_check_window_start(PPU *ppu);
void ppu_sprite_fifo_reset(SpriteFIFO *fifo);
void ppu_sprite_fifo_push(SpriteFIFO *fifo, SpritePixel pixel);
int ppu_sprite_fifo_pop(SpriteFIFO *fifo, SpritePixel *pixel);
void ppu_fetch_sprite(PPU *ppu, Sprite *sprite);
int ppu_get_sprite_fifo_pixel(PPU *ppu, int screen_x, SpritePixel *result);
void ppu_sprite_fetch_update(PPU *ppu);
uint8_t ppu_get_sprite_fetch_delay(PPU *ppu, Sprite *sprite);
void ppu_merge_sprite_pixel(PPU *ppu, SpritePixel pixel);
void ppu_sprite_fifo_remove_before(SpriteFIFO *fifo, int screen_x);