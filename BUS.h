//
// Created by Livia on 22.10.2026.
//

#pragma once

#define WRAM_SIZE 8192
#define HRAM_SIZE 127
#define ROM_SIZE 0x8000
#include <stdint.h>
#include "ppu.h"

typedef struct TIMER TIMER;

typedef enum {
    JOYPAD_RIGHT,
    JOYPAD_LEFT,
    JOYPAD_UP,
    JOYPAD_DOWN,
    JOYPAD_A,
    JOYPAD_B,
    JOYPAD_SELECT,
    JOYPAD_START
} JoypadButton;

typedef struct BUS {
    uint8_t rom[ROM_SIZE];
    uint8_t wram[WRAM_SIZE];
    uint8_t hram[HRAM_SIZE];
    uint8_t interrupt_enable; // IE: 0xFFFF
    uint8_t interrupt_flags;  // IF: 0xFF0F

    PPU *ppu;
    TIMER *timer;

    uint8_t dma_active;
    uint16_t dma_source;
    uint16_t dma_index;
    uint8_t dma_cycle_counter;
    uint8_t dma_startup_delay;

    //Serial Port
    uint8_t serial_data;
    uint8_t serial_control;

    //Buttons
    uint8_t joypad_select;
    uint8_t joypad_buttons;
    uint8_t joypad_directions;
    uint8_t stop_wake_requested;
} BUS;

void bus_init(BUS *BUS, PPU *ppu, TIMER *timer);
uint8_t bus_read_byte(BUS *BUS, uint16_t address);
void bus_write_byte(BUS *BUS, uint16_t address, uint8_t value);
void bus_update_dma(BUS *BUS, uint64_t cycles);
static uint8_t bus_read_joypad(BUS *BUS);
void bus_set_joypad_button(BUS *BUS, JoypadButton button, uint8_t pressed);

//GB_EMULATOR_BUS_H
