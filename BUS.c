//
// Created by Livia on 22.10.2026.
//

#include "BUS.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


//Read a byte from wram
uint8_t bus_read_byte(BUS *BUS, uint16_t address) {

    //Check Echo ram
    if (address >= 0xE000 && address <= 0xFDFF) {
        address -= 0x2000;
    }

    //Check WRAM
    if (address >= 0xC000 && address <= 0xDFFF) {
        uint16_t index = (address - 0xC000);
        return BUS->wram[index];
    }

    //Check HRAM
    if (address >= 0xFF80 && address <= 0xFFFE) {
        uint16_t index = address - 0xFF80;
        return BUS->hram[index];
    }

    if (address == 0xFFFF) {
        return BUS->interrupt_enable;
    }
    if (address == 0xFF0F) {
        return BUS->interrupt_flags | 0xE0;
    }

    return 0xFF;
}

//Write a byte into wram
void bus_write_byte(BUS *BUS, uint16_t address, uint8_t value) {

    if (address >= 0xE000 && address <= 0xFDFF) {
        address -= 0x2000;
    }

    if (address >= 0xC000 && address <= 0xDFFF) {
        uint16_t index = (address - 0xC000);
        BUS->wram[index] = value;
        return;
    }

    if (address >= 0xFF80 && address <= 0xFFFE) {
        uint16_t index = (address - 0xFF80);
        BUS->hram[index] = value;
        return;
    }

    if (address == 0xFFFF) {
        BUS->interrupt_enable = value;
        return;
    }

    if (address == 0xFF0F) {
        BUS->interrupt_flags = value & 0x1F;
        return;
    }
}

