//
// Created by Livia on 22.10.2026.
//

#pragma once

#define WRAM_SIZE 8192
#include <stdint.h>

typedef struct BUS {

    uint8_t wram[WRAM_SIZE];
} BUS;

uint8_t bus_read_byte(BUS *BUS, uint16_t address);
void bus_write_byte(BUS *BUS, uint16_t address, uint8_t value);

//GB_EMULATOR_BUS_H
