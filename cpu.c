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

        default:
            break;
    }

    (void)opcode;
}