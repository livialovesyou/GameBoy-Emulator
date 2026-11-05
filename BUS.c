//
// Created by Livia on 22.10.2026.
//

#include "BUS.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "timer.h"


static uint8_t bus_read_byte_internal(BUS *BUS, uint16_t address) {
    switch (address) {

        //timer regs
        case 0xFF04:
        case 0xFF05:
        case 0xFF06:
        case 0xFF07:
            return timer_read_register(BUS->timer, address);

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




    // VRAM: 0x8000–0x9FFF
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (!ppu_vram_accessible(BUS->ppu))
            return 0xFF;

        return BUS->ppu->vram[address - 0x8000];
    }

    // OAM: 0xFE00–0xFE9F
    if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (!ppu_oam_accessible(BUS->ppu))
            return 0xFF;

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

//Read a byte from wram
uint8_t bus_read_byte(BUS *BUS, uint16_t address) {

    // if (address >= 0xFF40 && address <= 0xFF4B) {
    //    return ppu_read_register(BUS->ppu, address);
    // }

    if (BUS->dma_active) {
        if (address < 0xFF80 || address > 0xFFFE) {
            return 0xFF;
        }
    }

    return bus_read_byte_internal(BUS, address);
}

uint8_t bus_read_byte_dma(BUS *BUS, uint16_t address) {
    return bus_read_byte_internal(BUS, address);
}

//Write a byte into wram
void bus_write_byte(BUS *BUS, uint16_t address, uint8_t value) {

    if (BUS->dma_active && address != 0xFF46) {
        if (address < 0xFF80 || address > 0xFFFE) {
            return;
        }
    }

    switch (address) {

        //timer regs
        case 0xFF04:
        case 0xFF05:
        case 0xFF06:
        case 0xFF07:
            timer_write_register(BUS->timer, address, value);
            break;

        case 0xFF46: {
            BUS->dma_active = 1;
            BUS->dma_source = (uint16_t)value << 8;
            BUS->dma_index = 0;
            BUS->dma_cycle_counter = 0;
            BUS->dma_startup_delay = 4;
            break;
        }

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

    // VRAM: 0x8000–0x9FFF
    if (address >= 0x8000 && address <= 0x9FFF)
    {
        if (!ppu_vram_accessible(BUS->ppu))
            return;

        BUS->ppu->vram[address - 0x8000] = value;
        return;
    }

    // OAM: 0xFE00–0xFE9F
    if (address >= 0xFE00 && address <= 0xFE9F)
    {
        if (!ppu_oam_accessible(BUS->ppu))
            return;

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

void bus_update_dma(BUS *BUS, uint64_t cycles) {
    if (!BUS->dma_active) {
        return;
    }

    for (uint64_t i = 0; i < cycles; i++) {
        if (BUS->dma_startup_delay > 0) {
            BUS->dma_startup_delay--;
            continue;
        }
        BUS->dma_cycle_counter++;

        if (BUS->dma_cycle_counter == 4) {
            BUS->dma_cycle_counter = 0;

            uint8_t data = bus_read_byte_dma(BUS, BUS->dma_source + BUS->dma_index);
            BUS->ppu->oam[BUS->dma_index] = data;
            BUS->dma_index++;

            if (BUS->dma_index == 0xA0) {
                BUS->dma_active = 0;
                break;
            }
        }
    }
}
