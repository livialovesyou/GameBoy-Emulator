//
// Created by Livia on 06.11.2026.
//

#pragma once

#include <stdint.h>

#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 144

#include <SDL3/SDL.h>

#include "BUS.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} DISPLAY;

int display_init(DISPLAY *display);
void display_destroy(DISPLAY *display);
void display_process_events(DISPLAY *display, int *running, BUS *BUS);
void display_render(
    DISPLAY *display,
    uint8_t framebuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH]
);
