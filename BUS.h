//
// Created by Livia on 22.10.2026.
//

#pragma once

#define WRAM_SIZE 8192
#define HRAM_SIZE 127
#include <stdint.h>

typedef struct BUS {

    uint8_t wram[WRAM_SIZE];
    uint8_t hram[HRAM_SIZE];
    uint8_t interrupt_enable; // IE: 0xFFFF
    uint8_t interrupt_flags;  // IF: 0xFF0F
} BUS;

uint8_t bus_read_byte(BUS *BUS, uint16_t address);
void bus_write_byte(BUS *BUS, uint16_t address, uint8_t value);

//GB_EMULATOR_BUS_H
