//
// Created by Livia on 23.10.2026.
//

#include "cpu.h"
#include "BUS.h"

static uint8_t cpu_fetch_byte(CPU *cpu, BUS *BUS) {
    uint8_t value = bus_read_byte(BUS, cpu->pc);
    cpu->pc++;
    return value;
}

void cpu_step(CPU *cpu, BUS *BUS) {
    uint8_t opcode = bus_read_byte(BUS, cpu->pc);
    cpu->pc++;

    switch (opcode) {
        case 0x00: // NOP
            break;

        case 0x3E: //LD A, d8
            uint8_t value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->a = value;
            break;
        case 0x06: //LD B, d8
            value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->b = value;
            break;

        case 0x0E:  //LD C, d8
            value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->c = value;
            break;

        case 0x16:  //LD D, d8
            value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->d = value;
            break;

        case 0x1E:  //LD E, d8
            value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->e = value;
            break;

        case 0x26:  //LD H, d8
            value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->h = value;
            break;

        case 0x2E:  //LD L,d8
            value = bus_read_byte(BUS, cpu->pc);
            cpu->pc++;
            cpu->l = value;
            break;

        case 0x47: //LD B, A // Loads A into B
            cpu->b = cpu->a;
            break;

        case 0x4F: //LD C, A // Loads A into C
            cpu->c = cpu->a;
            break;

        case 0x57: //LD D, A // Loads A into D
            cpu->d = cpu->a;
            break;

        case 0x5F:  //LD E, A // Loads A into E
            cpu->e = cpu->a;
            break;

        case 0x67:  //LD H, A // Loads A into H
            cpu->h = cpu->a;
            break;

        case 0x6F:  //LD L, A // Loads A into L
            cpu->l = cpu->a;
            break;

        case 0x78: //LD A, B // Loads B into A
            cpu->a = cpu->b;
            break;

        case 0x79: //LD A, C // Loads C into A
            cpu->a = cpu->c;
            break;

        case 0x7A: //LD A, D // Loads D into A
            cpu->a = cpu->d;
            break;

        case 0x7B: //LD A, E // Loads E into A
            cpu->a = cpu->e;
            break;

        case 0x7C: //LD A, H // Loads H into A
            cpu->a = cpu->h;
            break;

        case 0x7D: //LD A, L // Loads L into A
            cpu->a = cpu->l;
            break;

        case 0x7E: //LD A, (HL) // Loads byte at address HL into A
            cpu->a = bus_read_byte(BUS, cpu_get_HL(cpu));
            break;

        case 0x77: //LD (HL), A // Writes A into byte at address stored in HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->a);
            break;

        case 0x36: //LD (HL), d8 // Writes 8bit immediate value at address stored in HL
            value = cpu_fetch_byte(cpu, BUS);
            bus_write_byte(BUS, cpu_get_HL(cpu), value);
            break;

        case 0x46: //LD B, (HL) // Loads byte at address HL into B
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->b = value;
            break;

        case 0x4E: //LD C, (HL) // Loads byte at address HL into C
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->c = value;
            break;

        case 0x56: //LD D, (HL) // Loads byte at address HL into D
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->d = value;
            break;

        case 0x5E: //LD E, (HL) // Loads byte at address HL into E
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->e = value;
            break;

        case 0x66: //LD H, (HL) // Loads byte at address HL into H
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->h = value;
            break;

        case 0x6E: //LD L, (HL) // Loads byte at address HL into L
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->l = value;
            break;

        case 0x70: //LD (HL), B // Writes B into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->b);
            break;

        case 0x71: //LD (HL), C // Writes C into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->c);
            break;

        case 0x72: //LD (HL), D // Writes D into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->d);
            break;

        case 0x73: //LD (HL), E // Writes E into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->e);
            break;

        case 0x74: //LD (HL), H // Writes H into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->h);
            break;

        case 0x75: //LD (HL), L // Writes L into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->l);
            break;

        case 0x21: //LD HL, d16 // Load 16bit Little Endian immediate to HL
            uint8_t low = cpu_fetch_byte(cpu, BUS);
            uint8_t high = cpu_fetch_byte(cpu, BUS);
            uint16_t value_16 = ((uint16_t)high << 8 ) | low;
            cpu_set_HL(cpu, value_16);
            break;

        case 0x01: //LD BC, d16 //Load 16bit Little Endian immediate to BC
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = ((uint16_t)high << 8 ) | low;
            cpu_set_BC(cpu, value_16);
            break;

        case 0x11: //LD DE, d16 //Load 16bit Little Endian immediate to DE
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = ((uint16_t)high << 8 ) | low;
            cpu_set_DE(cpu, value_16);
            break;

        case 0x31: //LD SP, d16 // Load 16bit Little Endian immediate to stack pointer
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = ((uint16_t)high << 8 ) | low;
            cpu->sp = value_16;
            break;

        case 0x04: // INC B // Increment B, record flags
            uint8_t result = cpu->b + 1;
            cpu->b = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);
            break;

        case 0x0C: // INC C // Increment C, record flags
            result = cpu->c + 1;
            cpu->c = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);
            break;

        case 0x05: // DEC B // Decrement B, record flags
            uint8_t old_val = cpu->b;
            result = old_val - 1;
            cpu->b = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (old_val & 0x0F) == 0);
            break;

        case 0x0D: // DEC C// Decrement C, record flags
            old_val = cpu->c;
            result = old_val - 1;
            cpu->c = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (old_val & 0x0F) == 0);
            break;

        case 0x80: //ADD A, B // Add B to A, store in A, record all flags
            uint8_t a = cpu->a;
            uint8_t b = cpu->b;

            uint16_t result_16 = a + b;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (b & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;

        case 0x81: //ADD A, C // Add C to A, store in A, record all flags
            a = cpu->a;
            uint8_t c = cpu->c;

            result_16 = a + c;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (c & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;

        case 0x90: //SUB A, B // Sub B from A, store in A, record all flags
            a = cpu->a;
            b = cpu->b;

            result_16 = a - b;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (b & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < b);
            break;

        case 0x91: //SUB A, C // Sub C from A, store in A, record all flags
            a = cpu->a;
            c = cpu->c;

            result_16 = a - c;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (c & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < c);
            break;

        case 0xA0: //AND A, B //Bitwise AND between A and B, stores in A, record flags
            a = cpu->a;
            b = cpu->b;
            result = a & b;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;

        case 0xA1: //AND A, C //Bitwise AND between A and C, stores in A, record flags
            a = cpu->a;
            c = cpu->c;
            result = a & c;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;


        default:
            break;
    }

    (void)opcode;
}