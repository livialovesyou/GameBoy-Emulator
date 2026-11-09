//
// Created by Livia on 22.10.2026.
//

#include "BUS.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "timer.h"

void bus_init(BUS *BUS, PPU *ppu, TIMER *timer)
{
    memset(BUS, 0, sizeof(*BUS));

    BUS->ppu = ppu;
    BUS->timer = timer;
    BUS->joypad_select = 0x30;
    BUS->joypad_buttons = 0x0F;
    BUS->joypad_directions = 0x0F;
    BUS->stop_wake_requested = 0;
    BUS->mbc_type = 0;
    BUS->mbc3_rom_bank = 1;
    BUS->mbc3_ram_bank = 0;
    BUS->mbc3_ram_enabled = 0;
    BUS->mbc3_latch_value = 0;

    BUS->mbc1_rom_bank_low5 = 1;
    BUS->mbc1_bank_high2 = 0;
    BUS->mbc1_banking_mode = 0;
    BUS->mbc1_ram_enabled = 0;
}


static uint8_t bus_read_byte_internal(BUS *BUS, uint16_t address) {

    // Cartridge ROM: 0x0000–0x3FFF
    if (address <= 0x3FFF)
    {
        if (BUS->rom == NULL)
            return 0xFF;

        uint32_t bank = 0;

        if (BUS->mbc_type >= 0x01 && BUS->mbc_type <= 0x03 &&
            BUS->mbc1_banking_mode == 1)
        {
            bank = (uint32_t)BUS->mbc1_bank_high2 << 5;
        }

        uint32_t rom_address = bank * 0x4000 + address;

        if (rom_address >= BUS->rom_size)
            return 0xFF;

        return BUS->rom[rom_address];
    }

    // Cartridge ROM: 0x4000–0x7FFF
    if (address <= 0x7FFF)
    {
        if (BUS->rom == NULL)
            return 0xFF;

        uint32_t bank = 1;

        if (BUS->mbc_type >= 0x01 && BUS->mbc_type <= 0x03)
        {
            bank = ((uint32_t)BUS->mbc1_bank_high2 << 5) |
                   BUS->mbc1_rom_bank_low5;
        }
        else if (BUS->mbc_type >= 0x0F &&
                 BUS->mbc_type <= 0x13)
        {
            bank = BUS->mbc3_rom_bank;
        }

        uint32_t rom_address = bank * 0x4000 + (address - 0x4000);


        if (rom_address >= BUS->rom_size)
            return 0xFF;

        return BUS->rom[rom_address];
    }


    // Cartridge external RAM: 0xA000–0xBFFF
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        uint32_t ram_address = address - 0xA000;

        // MBC1
        if (BUS->mbc_type >= 0x01 && BUS->mbc_type <= 0x03)
        {
            if (!BUS->mbc1_ram_enabled)
                return 0xFF;

            if (BUS->mbc1_banking_mode == 1)
                ram_address += BUS->mbc1_bank_high2 * 0x2000;
        }
        // MBC3
        else if (BUS->mbc_type >= 0x0F && BUS->mbc_type <= 0x13)
        {
            if (!BUS->mbc3_ram_enabled || BUS->mbc3_ram_bank > 3)
                return 0xFF;

            ram_address += BUS->mbc3_ram_bank * 0x2000;
        }
        else
        {
            return 0xFF;
        }

        // // This cartridge has 8 KB of RAM.
        // if (ram_address >= 0x2000)
        //     return 0xFF;

        return BUS->cart_ram[ram_address];
    }


    if (address == 0xFF00) {
        return bus_read_joypad(BUS);
    }

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

        case 0xFF48:
            return BUS->ppu->obp0;

        case 0xFF49:
            return BUS->ppu->obp1;
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

    // Cartridge banking registers
    if (address <= 0x7FFF)
    {
        // MBC1
        if (BUS->mbc_type == 0x01 ||
            BUS->mbc_type == 0x02 ||
            BUS->mbc_type == 0x03)
        {
            if (address <= 0x1FFF)
            {
                // RAM enable
                BUS->mbc1_ram_enabled = ((value & 0x0F) == 0x0A);
            }
            else if (address <= 0x3FFF)
            {
                // ROM bank number (lower 5 bits)
                BUS->mbc1_rom_bank_low5 = value & 0x1F;

                if (BUS->mbc1_rom_bank_low5 == 0)
                    BUS->mbc1_rom_bank_low5 = 1;

            }
            else if (address <= 0x5FFF)
            {
                // Upper 2 bank bits
                BUS->mbc1_bank_high2 = value & 0x03;
                printf("MBC1 high bank bits: %02X\n",
                    BUS->mbc1_bank_high2);
            }
            else
            {
                // Banking mode
                BUS->mbc1_banking_mode = value & 0x01;
            }
        }
        // MBC3
        else if (BUS->mbc_type >= 0x0F &&
                 BUS->mbc_type <= 0x13)
        {
            if (address <= 0x1FFF)
            {
                BUS->mbc3_ram_enabled = ((value & 0x0F) == 0x0A);
            }
            else if (address <= 0x3FFF)
            {
                BUS->mbc3_rom_bank = value & 0x7F;

                if (BUS->mbc3_rom_bank == 0)
                    BUS->mbc3_rom_bank = 1;
            }
            else if (address <= 0x5FFF)
            {
                if (value <= 3)
                    BUS->mbc3_ram_bank = value;
                else
                    BUS->mbc3_ram_bank = 0xFF;
            }
            // RTC latch at 0x6000–0x7FFF is not implemented
        }

        return;
    }

    // Cartridge external RAM: 0xA000–0xBFFF
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        uint32_t ram_address = address - 0xA000;

        // MBC1
        if (BUS->mbc_type >= 0x01 && BUS->mbc_type <= 0x03)
        {
            if (!BUS->mbc1_ram_enabled)
                return;

            if (BUS->mbc1_banking_mode == 1)
                ram_address += BUS->mbc1_bank_high2 * 0x2000;
        }
        // MBC3
        else if (BUS->mbc_type >= 0x0F && BUS->mbc_type <= 0x13)
        {
            if (!BUS->mbc3_ram_enabled || BUS->mbc3_ram_bank > 3)
                return;

            ram_address += BUS->mbc3_ram_bank * 0x2000;
        }
        else
        {
            return;
        }

        if (ram_address < CART_RAM_SIZE)
            BUS->cart_ram[ram_address] = value;

        return;
    }


    switch (address) {

        case 0xFF00:
            BUS->joypad_select = value & 0x30;
            return;

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

        case 0xFF40:{ // LCDC
            //printf("LCDC write: %02X\n", value);

        }
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
        case 0xFF48:
            ppu_write_register(BUS->ppu, address, value);
            return;

        case 0xFF49:
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

static uint8_t bus_read_joypad(BUS *BUS)
{
    uint8_t result = 0x0F;

    if (!(BUS->joypad_select & 0x10)) {
        result &= BUS->joypad_directions;
    }

    if (!(BUS->joypad_select & 0x20)) {
        result &= BUS->joypad_buttons;
    }

    return 0xC0 | BUS->joypad_select | result;
}

void bus_set_joypad_button(BUS *BUS, JoypadButton button, uint8_t pressed) {
    uint8_t old_value = bus_read_joypad(BUS);
    uint8_t *register_value;
    uint8_t bit;

    if (button <= JOYPAD_DOWN) {
        register_value = &BUS->joypad_directions;
        bit = button;
    } else {
        register_value = &BUS->joypad_buttons;
        bit = button - JOYPAD_A;
    }
    uint8_t was_released = (*register_value & (1 << bit)) != 0;
    if (pressed && was_released) {
        BUS->stop_wake_requested = 1;
    }

    if (pressed) {
        *register_value &= ~(1 << bit);
    } else {
        *register_value |= (1 << bit);
    }

    uint8_t new_value = bus_read_joypad(BUS) & 0x0F;

    if (old_value & (uint8_t)~new_value & 0x0F) {
        BUS->interrupt_flags |= 0x10;
    }
}