//
// Created by Livia on 06.11.2026.
//

#include "display.h"

#include <stdio.h>

int display_init(DISPLAY *display) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 0;
    }

    display->window = SDL_CreateWindow("GB Emulator", 640, 576, 0);

    if (!display->window)
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    display->renderer = SDL_CreateRenderer(display->window, NULL);

    if (!display->renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return 0;
    }

    display->texture = SDL_CreateTexture(
        display->renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT);

    if (!display->texture) {
        printf("Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return 0;
    }
//     SDL_SetTextureScaleMode(
//     display->texture,
//     SDL_SCALEMODE_NEAREST
// );


    return 1;
}

void display_destroy(DISPLAY *display)
{
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}

void display_process_events(DISPLAY *display, int *running, BUS *BUS)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            *running = 0;
        }

        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            uint8_t pressed = (event.type == SDL_EVENT_KEY_DOWN);
            switch (event.key.key) {
                case SDLK_RIGHT:
                    bus_set_joypad_button(BUS,JOYPAD_RIGHT,pressed);
                    break;

                case SDLK_LEFT:
                    bus_set_joypad_button(BUS, JOYPAD_LEFT, pressed);
                    break;

                case SDLK_UP:
                    bus_set_joypad_button(BUS, JOYPAD_UP, pressed);
                    break;

                case SDLK_DOWN:
                    bus_set_joypad_button(BUS, JOYPAD_DOWN, pressed);
                    break;

                case SDLK_Z:
                    bus_set_joypad_button(BUS, JOYPAD_A, pressed);
                    break;

                case SDLK_X:
                    bus_set_joypad_button(BUS, JOYPAD_B, pressed);
                    break;

                case SDLK_BACKSPACE:
                    bus_set_joypad_button(BUS, JOYPAD_SELECT, pressed);
                    break;

                case SDLK_RETURN:
                    bus_set_joypad_button(BUS, JOYPAD_START, pressed);
                    break;

                default:
                    break;
            }
        }
    }
}
void display_render(
    DISPLAY *display,
    uint8_t framebuffer[DISPLAY_HEIGHT][DISPLAY_WIDTH])
{
    uint8_t pixels[DISPLAY_HEIGHT][DISPLAY_WIDTH][4];

    const uint8_t colors[4][4] = {
        {255, 255, 255, 255},
        {170, 170, 170, 255},
        {85, 85, 85, 255},
        {0, 0, 0, 255}
    };

    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            uint8_t color = framebuffer[y][x] & 0x03;

            for (int channel = 0; channel < 4; channel++)
                pixels[y][x][channel] = colors[color][channel];
        }
    }

    if (!SDL_UpdateTexture(display->texture, NULL, pixels, DISPLAY_WIDTH * 4))
        printf("SDL_UpdateTexture failed: %s\n", SDL_GetError());

    if (!SDL_RenderClear(display->renderer))
        printf("SDL_RenderClear failed: %s\n", SDL_GetError());

    if (!SDL_RenderTexture(display->renderer, display->texture, NULL, NULL))
        printf("SDL_RenderTexture failed: %s\n", SDL_GetError());

    SDL_RenderPresent(display->renderer);
}
