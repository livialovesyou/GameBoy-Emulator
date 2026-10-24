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

        case 0xB0: // OR A, B //Bitwise OR between A and B, stores in A, record flags
            a = cpu->a;
            b = cpu->b;
            result = a | b;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;

        case 0xB1: // OR A, C //Bitwise OR between A and C, stores in A, record flags
            a = cpu->a;
            c = cpu->c;
            result = a | c;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;

        case 0xA8: // XOR A, B //Bitwise XOR between A and B, stores in A, record flags
            a = cpu->a;
            b = cpu->b;
            result = a ^ b;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;

        case 0xA9: // XOR A, C //Bitwise XOR between A and C, stores in A, record flags
            a = cpu->a;
            c = cpu->c;
            result = a ^ c;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;

        case 0xB8: //CP A,B // Compare A and B, record flags
            a = cpu->a;
            b = cpu->b;

            cpu_set_flag(cpu, FLAG_Z, a == b);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (b & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < b);
            break;

        case 0xB9: //CP A,B // Compare A and B, record flags
            a = cpu->a;
            c = cpu->c;

            cpu_set_flag(cpu, FLAG_Z, a == c);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (c & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < c);
            break;

        case 0xC3: //JP, d16 // Unconditional jump to immediate 16bit address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;
            cpu->pc = value_16;
            break;

        case 0xC2: //JP NZ, d16 // Conditional jump to immediate 16bit address if ZERO Flag is cleared
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_Z) == 0) {

                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                break;
            } else {
                break;
            }

        case 0xCA: //JP Z, d16 // Conditional jump to immediate 16bit address if ZERO Flag is set
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_Z) == 1) {

                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                break;
            } else {
                break;
            }

        case 0xD2: //JP NC, d16 // Conditional jump to immediate 16bit address if CARRY Flag is cleared
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_C) == 0) {

                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                break;
            } else {
                break;
            }

        case 0xDA: //JP C, d16 // Conditional jump to immediate 16bit address if CARRY Flag is set
            if (cpu_flag_is_set(cpu, FLAG_C) == 1) {
                low = cpu_fetch_byte(cpu, BUS);
                high = cpu_fetch_byte(cpu, BUS);
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                break;
            } else {
                break;
            }

        case 0x18: //JR, d8 // Unconditional jump to 8bit offset
            int8_t offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            cpu->pc = (uint16_t)(cpu->pc + offset);
            break;

        case 0x20: //JR NZ,d8 // Conditional relative jump if ZERO FLAG is cleared
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_Z) == 0) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
            }
            break;

        case 0x28: //JR Z,d8 // Conditional relative jump if ZERO FLAG is set
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_Z) == 1) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
            }
            break;

        case 0x30: //JR NC,d8 // Conditional relative jump if CARRY FLAG is cleared
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_C) == 0) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
            }
            break;

        case 0x38: //JR C,d8 // Conditional relative jump if CARRY FLAG is set
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_C) == 1) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
            }
            break;

        case 0xCD: //CALL d16 // Calls subroutine, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;
            uint16_t return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = value_16;
            break;

        case 0xC9: //RET //Returns from subroutine by reading the saved return address from the stack and loading it into the program counter
            low = bus_read_byte(BUS, cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS, cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu->pc = value_16;
            break;

        case 0xC4: // //CALL NZ d16 // Calls subroutine when ZERO FLAG is cleared, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;

            if (cpu_flag_is_set(cpu, FLAG_Z) == 0) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
            }
            break;

        case 0xCC: // //CALL Z d16 // Calls subroutine when ZERO FLAG is set, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;

            if (cpu_flag_is_set(cpu, FLAG_Z) == 1) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
            }
            break;

        case 0xD4: // //CALL NC d16 // Calls subroutine when CARRY FLAG is cleared, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;

            if (cpu_flag_is_set(cpu, FLAG_C) == 0) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
            }
            break;

        case 0xDC: // //CALL C d16 // Calls subroutine when CARRY FLAG is set, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;

            if (cpu_flag_is_set(cpu, FLAG_C) == 1) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
            }
            break;

        case 0xC5:// PUSH BC // Writes the 16bit value into the stack
            value_16 = cpu_get_BC(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            break;

        case 0xD5:// PUSH DE // Writes the 16bit value into the stack
            value_16 = cpu_get_DE(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            break;

        case 0xE5:// PUSH HL // Writes the 16bit value into the stack
            value_16 = cpu_get_HL(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            break;

        case 0xF5:// PUSH AF // Writes the 16bit value into the stack
            value_16 = cpu_get_AF(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            break;

        case 0xC1: //POP BC // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_BC(cpu,value_16);
            break;

        case 0xD1: //POP DE // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_DE(cpu,value_16);
            break;

        case 0xE1: //POP HL // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_HL(cpu,value_16);
            break;

        case 0xF1: //POP AF // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_AF(cpu,value_16);
            break;

        case 0xC0: //RET NZ //Return only if ZERO FLAG is cleared
            if (cpu_flag_is_set(cpu,FLAG_Z) == 0) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
            }
            break;

        case 0xC8: //RET Z //Return only if ZERO FLAG is set
            if (cpu_flag_is_set(cpu,FLAG_Z) == 1) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
            }
            break;

        case 0xD0: //RET NC //Return only if CARRY FLAG is cleared
            if (cpu_flag_is_set(cpu,FLAG_C) == 0) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
            }
            break;

        case 0xD8: //RET C //Return only if ZERO FLAG is set
            if (cpu_flag_is_set(cpu,FLAG_C) == 1) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
            }
            break;

        case 0xD9: //RETI //Return from an interrupt handler and re enable interrupts
            low = bus_read_byte(BUS, cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS, cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu->pc = value_16;
            cpu->ime = 1;
            break;

        case 0xC7: //RST 00H // Jumps to 0x0000 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0000;
            break;

        case 0xCF: //RST 08H // Jumps to 0x0008 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0008;
            break;

        case 0xD7: //RST 10H // Jumps to 0x0010 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0010;
            break;

        case 0xDF: //RST 18H // Jumps to 0x0018 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0018;
            break;

        case 0xE7: //RST 20H // Jumps to 0x0020 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0020;
            break;

        case 0xEF: //RST 28H // Jumps to 0x0028 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0028;
            break;

        case 0xF7: //RST 30H // Jumps to 0x0030 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0030;
            break;

        case 0xFF: //RST 38H // Jumps to 0x0030 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0038;
            break;

        case 0x08: //LD (d16), SP writes to memory the stack pointer
            low = cpu_fetch_byte(cpu,BUS);
            high = cpu_fetch_byte(cpu,BUS);
            value_16 = ((uint16_t)high << 8) | low;
            uint16_t sp = cpu->sp;
            bus_write_byte(BUS,value_16, (uint8_t)(sp & 0xFF));
            bus_write_byte(BUS,value_16 + 1, (uint8_t)(sp >> 8));
            break;


        default:
            break;
    }

    (void)opcode;
}