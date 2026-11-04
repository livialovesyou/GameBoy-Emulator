//
// Created by Livia on 22.10.2026.
//

#include "BUS.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


//Read a byte from wram
uint8_t bus_read_byte(BUS *BUS, uint16_t address) {

    // if (address >= 0xFF40 && address <= 0xFF4B) {
    //    return ppu_read_register(BUS->ppu, address);
    // }

    switch (address) {
        case 0xFF40: // LCDC
        case 0xFF41: // STAT
        case 0xFF42: // SCY
        case 0xFF43: // SCX
        case 0xFF44: // LY
        case 0xFF45: // LYC
        case 0xFF47: // BGP
        case 0xFF4A: // WY
        case 0xFF4B: // WX
            return ppu_read_register(BUS->ppu, address);
    }


    //VRAM
    if (address >= 0x8000 && address <= 0x9FFF) {
        return BUS->ppu->vram[address - 0x8000];
    }

    //OAM
    if (address >= 0xFE00 && address <= 0xFE9F) {
        return BUS->ppu->oam[address - 0xFE00];
    }

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

    switch (address) {
        case 0xFF40: // LCDC
        case 0xFF41: // STAT
        case 0xFF42: // SCY
        case 0xFF43: // SCX
        case 0xFF44: // LY
        case 0xFF45: // LYC
        case 0xFF47: // BGP
        case 0xFF4A: // WY
        case 0xFF4B: // WX
            ppu_write_register(BUS->ppu, address, value);
            return;
    }


    if (address == REG_LCDC || address == REG_BGP) {
        ppu_write_register(BUS->ppu, address, value);
    }

    //VRAM
    if (address >= 0x8000 && address <= 0x9FFF) {
        BUS->ppu->vram[address - 0x8000] = value;
        return;
    }

    //OAM
    if (address >= 0xFE00 && address <= 0xFE9F) {
        BUS->ppu->oam[address - 0xFE00] = value;
        return;
    }

    if (address >= 0xE000 && address <= 0xFDFF) {
        address -= 0x2000;
        return;
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

