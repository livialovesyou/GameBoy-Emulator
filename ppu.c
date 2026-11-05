//
// Created by Livia on 04.11.2026.
//

#include "bus.h"
#include "ppu.h"
#include <stdint.h>
#include <string.h>

void ppu_init(PPU *ppu) {
    memset(ppu,0,sizeof(PPU));

    ppu->mode = PPU_MODE_OAM;
    ppu->previous_mode = PPU_MODE_OAM;
    ppu->ly = 0;
    ppu->dot = 0;

    ppu->window_line = 0;
    ppu->lcd_was_enabled = 0;
}

void ppu_decode_tile(PPU *ppu, uint16_t tile_address, uint8_t pixels[8][8]) {
    uint16_t offset = tile_address - 0x8000;

    for (int y = 0; y < 8; y++) {
        uint8_t low = ppu->vram[offset + y * 2];
        uint8_t high = ppu->vram[offset + y * 2 + 1];
        for (int x = 0; x < 8; x++) {
            int bit = 7 - x;

            uint8_t color =
                ((high >> bit) & 1) << 1 |
                    ((low >> bit) & 1);
            pixels[y][x] = color;
        }
    }
}

void ppu_render_background(PPU *ppu, uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH]) {

    if (!(ppu->lcdc & 0x01))
    {
        memset(pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
        return;
    }

    uint16_t map_address;
    if (ppu->lcdc & 0x08) {
        map_address = 0x9C00;
    } else {
        map_address = 0x9800;
    }

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {

            //Apply BG scrolling
            uint8_t bg_x = (uint8_t)(x + ppu->scx);
            uint8_t bg_y = (uint8_t)(y + ppu->scy);

            //Find tile coordinates
            uint8_t tile_x = bg_x / 8;
            uint8_t tile_y = bg_y / 8;

            //Find tile position in the map
            uint16_t map_offset = tile_y * 32 + tile_x;
            uint8_t tile_number = ppu->vram[(map_address - 0x8000) + map_offset];

            //Determine tile data address
            uint16_t tile_address;

            if (ppu->lcdc & 0x10) {
                //unsigned
                tile_address = 0x8000 + tile_number * 16;
            } else {
                //signed
                tile_address = 0x9000 +(int8_t)tile_number * 16;
            }

            //Decode tile
            uint8_t tile_pixels[8][8];
            ppu_decode_tile(ppu, tile_address, tile_pixels);

            //Find the pixel inside the tile
            uint8_t pixel_x = bg_x % 8;
            uint8_t pixel_y = bg_y % 8;

            //Store its color index
            uint8_t color_index = tile_pixels[pixel_y][pixel_x];
            pixels[y][x] = ppu_get_bg_color(ppu, color_index);

        }
    }
}

uint8_t ppu_get_bg_color(PPU *ppu, uint8_t color_index) {
    uint8_t shift = color_index * 2;
    return (ppu->bgp >> shift) & 0x03;
}

uint8_t ppu_read_register(PPU *ppu, uint16_t address) {
    switch (address) {
        case REG_LCDC:
            return ppu->lcdc;

        case REG_BGP:
            return ppu->bgp;

        case 0xFF41: return ppu->stat | 0x80;
        case 0xFF42: return ppu->scy;
        case 0xFF43: return ppu->scx;
        case 0xFF44: return ppu->ly;
        case 0xFF45: return ppu->lyc;
        case 0xFF4A: return ppu->wy;
        case 0xFF4B: return ppu->wx;

        default:
            return 0xFF;
    }
}

void ppu_write_register(PPU *ppu, uint16_t address, uint8_t value) {
    switch (address) {
        case REG_LCDC:
            ppu->lcdc = value;
            break;

        case REG_BGP:
            ppu->bgp = value;
            break;

        case 0xFF41: ppu->stat = (ppu->stat & 0x07) | (value & 0x78); break;
        case 0xFF42: ppu->scy = value; break;
        case 0xFF43: ppu->scx = value; break;
        case 0xFF44: ppu->ly = 0; break;
        case 0xFF45:
            ppu->lyc = value;
            ppu_update_stat(ppu);
            break;
        case 0xFF4A: ppu->wy = value; break;
        case 0xFF4B: ppu->wx = value; break;

        default:
            break;
    }
}

// void ppu_update(PPU *ppu, BUS *BUS, uint64_t cycles, uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
//
//     //Sreen is turned off
//     if (!(ppu->lcdc & 0x80)) {
//         ppu->ly = 0;
//         ppu->dot = 0;
//         ppu->mode = PPU_MODE_HBLANK;
//         ppu_update_stat(ppu);
//         ppu_request_stat_interrupt(ppu, BUS);
//         return;
//     }
//     ppu->dot += cycles;
//
//     // Advance completed scanlines
//     while (cycles > 0)
//     {
//         uint16_t boundary;
//
//         //Get next boundary
//
//         ppu->dot -= 456;
//         ppu->ly++;
//
//         if (ppu->ly == 144)
//             BUS->interrupt_flags |= INT_VBLANK;
//
//
//         if (ppu->ly >= 154)
//         {
//             ppu->ly = 0;
//         }
//     }
//
//     //Determine current mode
//     if (ppu->ly >= 144) {
//         ppu->mode = PPU_MODE_VBLANK;
//     } else if (ppu->dot < 80) {
//         ppu->mode = PPU_MODE_OAM;
//     } else if (ppu->dot < 252) {
//         ppu->mode = PPU_MODE_DRAWING;
//     } else {
//         ppu->mode = PPU_MODE_HBLANK;
//     }
//
//     if (ppu->previous_mode == PPU_MODE_DRAWING &&
//     ppu->mode == PPU_MODE_HBLANK)
//     {
//         ppu_render_scanline(ppu, framebuffer);
//     }
//
//     ppu->previous_mode = ppu->mode;
//
//     if (ppu->dot >= 456) {
//         ppu->dot -= 456;
//         ppu->ly++;
//         if (ppu->ly >= 154) {
//             ppu->ly = 0;
//         }
//     }
//     ppu_update_stat(ppu);
//     ppu_request_stat_interrupt(ppu, BUS);
// }

void ppu_update(PPU *ppu, BUS *BUS, uint64_t cycles,
                uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH])
{

    int lcd_enabled = (ppu->lcdc & 0x80) != 0;

    // LCD was just disabled.
    if (!lcd_enabled)
    {
        ppu->ly = 0;
        ppu->dot = 0;
        ppu->mode = PPU_MODE_HBLANK;
        ppu->window_line = 0;
        ppu->stat_irq_line = 0;
        ppu->lcd_was_enabled = 0;
        ppu_update_stat(ppu);
        return;
    }

    // LCD was just enabled.
    if (!ppu->lcd_was_enabled) {
        ppu->ly = 0;
        ppu->dot = 0;
        ppu->mode = PPU_MODE_OAM;
        ppu->window_line = 0;
        ppu->stat_irq_line = 0;
        ppu->lcd_was_enabled = 1;

        ppu_update_stat(ppu);
        ppu_request_stat_interrupt(ppu, BUS);
    }

    while (cycles > 0)
    {
        uint16_t boundary;

        // Determine the next boundary.
        if (ppu->ly >= 144)
        {
            boundary = 456;
        }
        else if (ppu->dot < 80)
        {
            boundary = 80;
        }
        else if (ppu->dot < 252)
        {
            boundary = 252;
        }
        else
        {
            boundary = 456;
        }

        // Calculate how many dots remain until the boundary.
        uint16_t dots_remaining = boundary - ppu->dot;

        // Advance only as far as the boundary or remaining cycles.
        uint16_t step = cycles < dots_remaining
                      ? (uint16_t)cycles
                      : dots_remaining;

        ppu->dot += step;
        cycles -= step;

        // Check whether we reached a boundary.
        if (ppu->dot == boundary)
        {
            if (boundary == 80)
            {
                ppu->mode = PPU_MODE_DRAWING;
            }
            else if (boundary == 252)
            {
                ppu->mode = PPU_MODE_HBLANK;

                // Render the completed scanline.
                ppu_render_scanline(ppu, framebuffer);
            }
            else if (boundary == 456)
            {
                ppu->dot = 0;
                ppu->ly++;

                if (ppu->ly == 144)
                    BUS->interrupt_flags |= INT_VBLANK;

                if (ppu->ly >= 154) {
                    ppu->ly = 0;
                    ppu->window_line = 0;
                }

                if (ppu->ly >= 144)
                    ppu->mode = PPU_MODE_VBLANK;
                else
                    ppu->mode = PPU_MODE_OAM;
            }
        }

        // Refresh STAT after each transition or partial advance.
        ppu_update_stat(ppu);
        ppu_request_stat_interrupt(ppu, BUS);
    }
}

void ppu_update_stat(PPU *ppu) {
    //Update coincidence flag (STAT bit 2)
    if (ppu->ly == ppu->lyc) {
        ppu->stat |= 0x04;
    } else {
        ppu->stat &= (uint8_t)~0x04;
    }
    // Update current PPU mode (STAT bits 0-1)
    ppu->stat = (ppu->stat & 0xFC) | ppu->mode;


}

void ppu_request_stat_interrupt(PPU *ppu, BUS *BUS) {
    int stat_line = 0;

    // STAT interrupts are disabled when the LCD is off.
    if (ppu->lcdc & 0x80) {

        // Mode 0: HBlank
        if (ppu->mode == PPU_MODE_HBLANK &&
            (ppu->stat & STAT_INT_HBLANK)) {
            stat_line = 1;
            }

        // Mode 1: VBlank
        if (ppu->mode == PPU_MODE_VBLANK &&
            (ppu->stat & STAT_INT_VBLANK)) {
            stat_line = 1;
            }

        // Mode 2: OAM scan
        if (ppu->mode == PPU_MODE_OAM &&
            (ppu->stat & STAT_INT_OAM))
            stat_line = 1;


        // LYC coincidence
        if ((ppu->stat & STAT_INT_LYC) &&
            (ppu->stat & 0x04))
            stat_line = 1;
    }

    if (stat_line && !ppu->stat_irq_line) {
        BUS->interrupt_flags |= INT_STAT;
    }
    // Remember the current signal state.
    ppu->stat_irq_line = stat_line;

}

// void ppu_render_scanline(PPU *ppu, uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH]) {
//     if (ppu->ly >= SCREEN_HEIGHT) {
//         return;
//     }
//
//     int y = ppu->ly;
//
//
//     if (!(ppu->lcdc & 0x01)) {
//         memset(pixels[y], 0, SCREEN_WIDTH);
//         return;
//     }
//
//     uint16_t map_address;
//
//     if (ppu->lcdc & 0x08) {
//         map_address = 0x9C00;
//     } else {
//         map_address = 0x9800;
//     }
//     int window_rendered = 0;
//
//
//     uint8_t bg_color_indices[SCREEN_WIDTH];
//
//     for (int x = 0; x < SCREEN_WIDTH; x++) {
//         uint8_t bg_x = (uint8_t)(x + ppu->scx);
//         uint8_t bg_y = (uint8_t)(y + ppu->scy);
//
//         uint8_t tile_x = bg_x / 8;
//         uint8_t tile_y = bg_y / 8;
//
//         uint16_t map_offset = tile_y * 32 + tile_x;
//
//         uint8_t tile_number = ppu->vram[(map_address - 0x8000) + map_offset];
//         uint16_t tile_address;
//
//         if (ppu->lcdc & 0x10) {
//             tile_address = 0x8000 + tile_number * 16;
//         } else {
//             tile_address = 0x9000 + (int8_t)tile_number * 16;
//         }
//         uint8_t tile_pixels[8][8];
//         ppu_decode_tile(ppu, tile_address, tile_pixels);
//
//         uint8_t pixel_x = bg_x % 8;
//         uint8_t pixel_y = bg_y % 8;
//
//         uint8_t color_index = tile_pixels[pixel_y][pixel_x];
//         // pixels[y][x] = ppu_get_bg_color(ppu, color_index);
//
//         // Convert Background color index to shade.
//         uint8_t color = ppu_get_bg_color(ppu, color_index);
//
//         //Render Window
//         if (ppu_window_is_visible(ppu, x, ppu->ly)) {
//             uint8_t w_color_index = ppu_get_window_pixel(ppu, x);
//             color_index = ppu_get_window_pixel(ppu, x);
//             color = ppu_get_bg_color(ppu, w_color_index);
//             window_rendered = 1;
//         }
//
//         bg_color_indices[x] = color_index;
//
//         //Store final pixel
//         pixels[y][x] = color;
//     }
//
//     //Render Sprite
//     ppu_render_sprites(ppu, y, pixels[y], bg_color_indices);
//
//     if (window_rendered)
//         ppu->window_line++;
//
// }


void ppu_render_scanline(PPU *ppu, uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH])
{
    if (ppu->ly >= SCREEN_HEIGHT)
        return;

    int y = ppu->ly;
    uint8_t bg_color_indices[SCREEN_WIDTH] = {0};
    int window_rendered = 0;

    // Background and Window rendering.
    if (ppu->lcdc & 0x01)
    {
        uint16_t map_address;

        if (ppu->lcdc & 0x08)
            map_address = 0x9C00;
        else
            map_address = 0x9800;

        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            uint8_t bg_x = (uint8_t)(x + ppu->scx);
            uint8_t bg_y = (uint8_t)(y + ppu->scy);

            uint8_t tile_x = bg_x / 8;
            uint8_t tile_y = bg_y / 8;

            uint16_t map_offset = tile_y * 32 + tile_x;

            uint8_t tile_number =
                ppu->vram[(map_address - 0x8000) + map_offset];

            uint16_t tile_address;

            if (ppu->lcdc & 0x10)
                tile_address = 0x8000 + tile_number * 16;
            else
                tile_address = 0x9000 + (int8_t)tile_number * 16;

            uint8_t tile_pixels[8][8];
            ppu_decode_tile(ppu, tile_address, tile_pixels);

            uint8_t pixel_x = bg_x % 8;
            uint8_t pixel_y = bg_y % 8;

            uint8_t color_index = tile_pixels[pixel_y][pixel_x];

            // Window rendering.
            if (ppu_window_is_visible(ppu, x, ppu->ly))
            {
                color_index = ppu_get_window_pixel(ppu, x);
                window_rendered = 1;
            }

            bg_color_indices[x] = color_index;
            pixels[y][x] = ppu_get_bg_color(ppu, color_index);
        }

        if (window_rendered)
            ppu->window_line++;
    }
    else
    {
        // Background and Window are disabled.
        // Their color index is zero across the scanline.
        memset(pixels[y], ppu_get_bg_color(ppu, 0), SCREEN_WIDTH);
    }

    // Sprites are rendered independently of LCDC bit 0.
    ppu_render_sprites(ppu, y, pixels[y], bg_color_indices);
}

int ppu_window_is_visible(PPU *ppu, int x, int y) {
    // Window must be enabled (LCDC bit 5).
    if (!(ppu->lcdc & 0x20))
        return 0;

    // Background and Window must be enabled (LCDC bit 0).
    if (!(ppu->lcdc & 0x01))
        return 0;

    // Window cannot appear above its WY position.
    if (y < ppu->wy)
        return 0;

    // WX is offset by 7 pixels.
    int window_x = (int)ppu->wx - 7;

    // Window must start before the right edge of the screen.
    if (window_x >= SCREEN_WIDTH)
        return 0;

    // Pixel must be at or to the right of the Window's start.
    if (x < window_x)
        return 0;

    return 1;
}

void ppu_get_window_coordinates(PPU *ppu, int x, uint8_t *tile_x, uint8_t *tile_y, uint8_t *pixel_x, uint8_t *pixel_y) {
    // Convert screen X to Window X.
    int window_x = x - ((int)ppu->wx - 7);

    // Window coordinates use window_line vertically.
    int window_y = ppu->window_line;

    // Determine which tile contains this pixel.
    *tile_x = window_x / 8;
    *tile_y = window_y / 8;

    // Determine the pixel inside that tile.
    *pixel_x = window_x % 8;
    *pixel_y = window_y % 8;
}

uint8_t ppu_get_window_pixel(PPU *ppu, int x) {
    uint8_t tile_x, tile_y;
    uint8_t pixel_x, pixel_y;

    ppu_get_window_coordinates(ppu, x, &tile_x, &tile_y, &pixel_x, &pixel_y);

    // Select the Window tile map.
    uint16_t map_address;

    if (ppu->lcdc & 0x40)
        map_address = 0x9C00;
    else
        map_address = 0x9800;

    // Find the tile number in the map.
    uint16_t map_offset = tile_y * 32 + tile_x;

    uint8_t tile_number = ppu->vram[(map_address - 0x8000) + map_offset];

    // Determine the tile's address.
    uint16_t tile_address;

    if (ppu->lcdc & 0x10) {
        tile_address = 0x8000 + tile_number * 16;
    } else {
        tile_address = 0x9000 + (int8_t)tile_number * 16;
    }

    //Decode the tile
    uint8_t tile_pixels[8][8];
    ppu_decode_tile(ppu, tile_address, tile_pixels);

    //Return pixel's color index
    return tile_pixels[pixel_y][pixel_x];
}

void ppu_get_sprite(PPU *ppu, int index, Sprite *sprite) {
    int offset = index * OAM_SPRITE_SIZE;
    sprite->y = ppu->oam[offset];
    sprite->x = ppu->oam[offset + 1];
    sprite->tile = ppu->oam[offset + 2];
    sprite->attributes = ppu->oam[offset + 3];
    sprite->oam_index = index;
}

int ppu_sprite_overlaps_scanline(PPU *ppu, Sprite *sprite, int scanline) {
    //Get sprite height
    int sprite_height;
    if (ppu->lcdc & 0x04) {
        sprite_height = 16;
    } else {
        sprite_height = 8;
    }

    // Convert hardware Y coordinate to screen coordinate.
    int sprite_y = sprite->y - 16;

    // Check whether the scanline intersects the sprite.
    if (scanline >= sprite_y && scanline < sprite_y + sprite_height) {
        return 1;
    }
    return 0;
}

int ppu_get_scanline_sprites(PPU *ppu, int scanline, Sprite sprites[MAX_SPRITES_PER_SCANLINE]) {
    int count = 0;

    for (int i = 0; i < OAM_SPRITE_COUNT; i++) {
        Sprite sprite;
        ppu_get_sprite(ppu, i, &sprite);

        if (ppu_sprite_overlaps_scanline(ppu, &sprite, scanline)) {
            sprites[count] = sprite;
            count++;

            if (count == MAX_SPRITES_PER_SCANLINE) {
                break;
            }
        }
    }
    return count;
}

uint8_t ppu_get_sprite_pixel(PPU *ppu, Sprite *sprite, int pixel_x, int pixel_y) {

    int sprite_height = (ppu->lcdc & 0x04) ? 16 : 8;

    // Apply horizontal flipping.
    if (sprite->attributes & 0x20) {
        pixel_x = 7 - pixel_x;
    }
    //Vertical flipping
    if (sprite->attributes & 0x40) {
        pixel_y = sprite_height - 1 - pixel_y;
    }

    uint8_t tile_number = sprite->tile;

    if (sprite_height == 16) {
        tile_number &= 0xFE;
        if (pixel_y >=8 ) {
            tile_number++;
            pixel_y -= 8;
        }
    }

    //Decode sprite tile
    uint8_t tile_pixels[8][8];
    uint16_t tile_address = 0x8000 + tile_number * 16;
    ppu_decode_tile(ppu, tile_address, tile_pixels);
    return tile_pixels[pixel_y][pixel_x];

}

void ppu_render_sprites(PPU *ppu, int scanline, uint8_t pixels[SCREEN_WIDTH], uint8_t bg_color_indices[SCREEN_WIDTH]) {

    if (!(ppu->lcdc & 0x02)) {
        return;
    }

    Sprite sprites[MAX_SPRITES_PER_SCANLINE];
    int count = ppu_get_scanline_sprites(ppu, scanline, sprites);\
    ppu_sort_sprites(sprites, count);
    int sprite_height = (ppu->lcdc & 0x04) ? 16 : 8;

    uint8_t sprite_claimed[SCREEN_WIDTH] = {0};

    for (int i = 0; i < count; i++) {
        Sprite *sprite = &sprites[i];
        int sprite_x = sprite->x - 8;
        int sprite_y = sprite->y - 16;
        int pixel_y = scanline - sprite_y;

        for (int x = 0; x < 8; x++) {
            int screen_x = sprite_x + x;
            if (screen_x < 0 || screen_x >= SCREEN_WIDTH) {
                continue;
            }
            uint8_t color_index = ppu_get_sprite_pixel(ppu, sprite, x, pixel_y);

            if (color_index == 0) {
                continue;
            }

            if (sprite_claimed[screen_x])
                continue;

            sprite_claimed[screen_x] = 1;

            // Bit 7: sprite is behind nonzero Background/Window pixels.
            if ((sprite->attributes & 0x80) &&
                bg_color_indices[screen_x] != 0)
            {
                continue;
            }

            pixels[screen_x] = ppu_get_sprite_color(ppu, sprite, color_index);
        }
    }
}

uint8_t ppu_get_sprite_color(PPU *ppu, Sprite *sprite, uint8_t color_index) {
    // Color index 0 is transparent.
    if (color_index == 0)
        return 0;

    uint8_t palette;

    if (sprite->attributes & 0x10) {
        palette = ppu->obp1;
    } else {
        palette = ppu->obp0;
    }

    uint8_t shift = color_index * 2;
    return (palette >> shift) & 0x03;
}

void ppu_sort_sprites(Sprite sprites[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (sprites[j].x < sprites[i].x) {
                Sprite temp = sprites[i];
                sprites[i] = sprites[j];
                sprites[j] = temp;
            }
        }
    }
}

int ppu_vram_accessible(PPU *ppu)
{
    if (!(ppu->lcdc & 0x80))
        return 1;

    return ppu->mode != PPU_MODE_DRAWING;
}

int ppu_oam_accessible(PPU *ppu)
{
    if (!(ppu->lcdc & 0x80))
        return 1;

    return ppu->mode != PPU_MODE_OAM &&
           ppu->mode != PPU_MODE_DRAWING;
}