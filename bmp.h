//
// Created by Livia on 05.11.2026.
//
#pragma once

#define BMP_WIDTH 160
#define BMP_HEIGHT 144

#define BYTES_PER_PIXEL 3
#define ROW_SIZE (BMP_WIDTH * BYTES_PER_PIXEL)
#define PIXEL_DATA_SIZE (ROW_SIZE * BMP_HEIGHT)
#define FILE_SIZE (54 + PIXEL_DATA_SIZE)

#include <stdint.h>
void bmp_save(const char* filename, uint8_t framebuffer[144][160]);
static uint8_t shade_to_rgb(uint8_t shade);