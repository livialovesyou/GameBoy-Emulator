//
// Created by Livia on 05.11.2026.
//

#include "bmp.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "bmp.h"



static uint8_t shade_to_rgb(uint8_t shade) {
    switch (shade) {
        case 0: return 255;
        case 1: return 170;
        case 2: return 85;
        case 3: return 0;
        default: return 255;
    }
}

void bmp_save (const char* filename, uint8_t framebuffer[BMP_HEIGHT][BMP_WIDTH]) {
    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("ERROR: could not open file for writing\n");
        return;
    }

    //BMP header
    uint8_t file_header[14] = {0};
    file_header[0] = 'B';
    file_header[1] = 'M';

    // File size (bytes 2-5).
    file_header[2] = FILE_SIZE;
    file_header[3] = FILE_SIZE >> 8;
    file_header[4] = FILE_SIZE >> 16;
    file_header[5] = FILE_SIZE >> 24;

    // Pixel data starts at byte 54.
    file_header[10] = 54;

    // BMP information header.
    uint8_t info_header[40] = {0};

    info_header[0] = 40;

    // Width: 160 pixels.
    info_header[4] = BMP_WIDTH;

    // Height: 144 pixels.
    info_header[8] = BMP_HEIGHT;

    // Number of color planes: 1.
    info_header[12] = 1;

    // Bits per pixel: 24.
    info_header[14] = 24;

    // Compression: BI_RGB (none).
    info_header[16] = 0;

    // Image data size.
    info_header[20] = PIXEL_DATA_SIZE;
    info_header[21] = PIXEL_DATA_SIZE >> 8;
    info_header[22] = PIXEL_DATA_SIZE >> 16;
    info_header[23] = PIXEL_DATA_SIZE >> 24;

    fwrite(file_header, sizeof(file_header), 1, fp);
    fwrite(info_header, sizeof(info_header), 1, fp);

    for (int y = BMP_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < BMP_WIDTH; x++) {
            uint8_t shade = framebuffer[y][x];
            uint8_t color = shade_to_rgb(shade);

            uint8_t pixel[3] = {
                color, //Blue
                color,  //Green
                color   //Red
            };
            fwrite(pixel, sizeof(pixel), 1, fp);
        }
    }



    fclose(fp);
}