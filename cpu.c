//
// Created by Livia on 23.10.2026.
//

#include "cpu.h"

#include <stdio.h>

#include "BUS.h"
#include "timer.h"

void cpu_init(CPU *cpu) {
    cpu->a = 0x01;
    cpu->f = 0xB0;

    cpu->b = 0x00;
    cpu->c = 0x13;

    cpu->d = 0x00;
    cpu->e = 0xD8;

    cpu->h = 0x01;
    cpu->l = 0x4D;

    cpu->sp = 0xFFFE;
    cpu->pc = 0x0100;

    cpu->ime = 0;
    cpu->ime_scheduled = 0;

    cpu->halted = 0;
    cpu->stopped = 0;
    cpu->halt_bug = 0;

    cpu->cycles = 0;
}


static uint8_t cpu_fetch_byte(CPU *cpu, BUS *BUS) {
    uint8_t value = bus_read_byte(BUS, cpu->pc);
    cpu->pc++;

    return value;
}



static uint8_t cpu_cb_read_target(CPU *cpu, BUS *bus, uint8_t target)
{
    switch (target) {
        case 0: return cpu->b;
        case 1: return cpu->c;
        case 2: return cpu->d;
        case 3: return cpu->e;
        case 4: return cpu->h;
        case 5: return cpu->l;
        case 6: return bus_read_byte(bus, cpu_get_HL(cpu));
        case 7: return cpu->a;
        default: return 0;
    }
}


static void cpu_cb_write_target(CPU *cpu, BUS *BUS,
                                uint8_t target, uint8_t value)
{
    switch (target) {
        case 0: cpu->b = value; break;
        case 1: cpu->c = value; break;
        case 2: cpu->d = value; break;
        case 3: cpu->e = value; break;
        case 4: cpu->h = value; break;
        case 5: cpu->l = value; break;
        case 6: bus_write_byte(BUS, cpu_get_HL(cpu), value); break;
        case 7: cpu->a = value; break;
    }
}


void cpu_execute_cb(CPU *cpu, BUS *BUS, uint8_t opcode)
{
    uint8_t group = opcode >> 6;
    uint8_t operation = (opcode >> 3) & 0x07;
    uint8_t target = opcode & 0x07;

    uint8_t value = cpu_cb_read_target(cpu, BUS, target);
    uint8_t result;
    uint8_t carry;

    switch (group) {
        case 0: // Rotates and shifts
            switch (operation) {
                case 0: // RLC r: Rotate left circular
                    carry = (value >> 7) & 1;
                    result = (uint8_t)((value << 1) | carry);
                    break;

                case 1: // RRC r: Rotate right circular
                    carry = value & 1;
                    result = (uint8_t)((value >> 1) | (carry << 7));
                    break;

                case 2: { // RL r: Rotate left through carry
                    uint8_t old_carry =
                        cpu_flag_is_set(cpu, FLAG_C);
                    carry = (value >> 7) & 1;
                    result = (uint8_t)((value << 1) | old_carry);
                    break;
                }

                case 3: { // RR r: Rotate right through carry
                    uint8_t old_carry =
                        cpu_flag_is_set(cpu, FLAG_C);
                    carry = value & 1;
                    result = (uint8_t)((value >> 1)
                                       | (old_carry << 7));
                    break;
                }

                case 4: // SLA r: Shift left arithmetic
                    carry = (value >> 7) & 1;
                    result = (uint8_t)(value << 1);
                    break;

                case 5: // SRA r: Shift right, preserve bit 7
                    carry = value & 1;
                    result = (uint8_t)((value >> 1) | (value & 0x80));
                    break;

                case 6: // SWAP r: Swap upper and lower nibbles
                    result = (uint8_t)((value << 4) | (value >> 4));
                    carry = 0;
                    break;

                case 7: // SRL r: Shift right logical
                    carry = value & 1;
                    result = (uint8_t)(value >> 1);
                    break;

                default:
                    return;
            }

            cpu_cb_write_target(cpu, BUS, target, result);

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, carry);
            break;

        case 1: // BIT b,r: Test bit; do not modify target
            cpu_set_flag(cpu, FLAG_Z,
                         (value & (1u << operation)) == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            // Carry flag is preserved.
            break;

        case 2: // RES b,r: Clear bit; flags unchanged
            result = (uint8_t)(value & ~(1u << operation));
            cpu_cb_write_target(cpu, BUS, target, result);
            break;

        case 3: // SET b,r: Set bit; flags unchanged
            result = (uint8_t)(value | (1u << operation));
            cpu_cb_write_target(cpu, BUS, target, result);
            break;

        default:
            break;
    }
}

static int cpu_service_interrupt(CPU *cpu, BUS *BUS) {
    if (!cpu->ime) {
        return 0;
    }
    //Find Enabled and requested interrupts
    uint8_t ie = bus_read_byte(BUS, 0xFFFF);
    uint8_t flags = bus_read_byte(BUS, 0xFF0F);
    uint8_t pending = ie & flags & 0x1F;

    if (pending == 0) {
        return 0;
    }

    //find highest priority pending interrupt

    uint8_t bit = 0;
    while ((pending & (1u << bit)) == 0) {
        bit++;
    }

    static const uint16_t vectors[5] = {
        0x0040, // V-Blank
        0x0048, // LCD STAT
        0x0050, // Timer
        0x0058, // Serial
        0x0060  // Joypad
    };
    cpu->ime = 0;

    //Clear selected interrupt request bit
    flags &= (uint8_t) ~ (1u << bit);
    bus_write_byte(BUS, 0xFF0F, flags);

    //Save current Program Counter to the SP
    cpu->sp--;
    bus_write_byte(BUS, cpu->sp, (uint8_t)(cpu->pc >> 8));
    cpu->sp--;
    bus_write_byte(BUS, cpu->sp, (uint8_t)(cpu->pc & 0xFF));

    //begin vector execution
    cpu->pc = vectors[bit];
    return 1;
}

void cpu_step(CPU *cpu, BUS *BUS) {
    uint8_t cycle = 4;

    if (cpu->stopped) {
        if (BUS->stop_wake_requested) {
            cpu->stopped = 0;
            BUS->stop_wake_requested = 0;
        } else {
            cpu->cycles += 4;
            return;
        }
    }


    if (cpu_service_interrupt(cpu, BUS)) {
        cpu->cycles += 20;
        return;
    }

    if (cpu->halted) {
        uint8_t ie = bus_read_byte(BUS, 0xFFFF);
        uint8_t flags = bus_read_byte(BUS, 0xFF0F);
        uint8_t pending = ie & flags & 0x1F;

        if (pending == 0) {
            cpu->cycles += cycle;
            return;
        }

        cpu->halted = 0;
    }

    if (cpu->ime_scheduled == 1) {
        cpu->ime = 1;
        cpu->ime_scheduled = 0;
    }


    uint8_t opcode = bus_read_byte(BUS, cpu->pc);
    if (cpu->halt_bug) {
        cpu->halt_bug = 0;
    } else {
        cpu->pc++;
    }

    switch (opcode) {
        case 0x00: // NOP
            break;

        case 0x10: // STOP
        {
            cpu_fetch_byte(cpu, BUS);
            timer_write_register(BUS->timer, 0xFF04, 0);
            cpu->stopped = 1;
            break;
        }

        case 0x40: //LD B, B
        {
            cpu->b = cpu->b;
            break;
        }

        case 0x50: //LD D, B
        {
            cpu->d = cpu->b;
            break;
        }

        case 0x60: //LD H, B
        {
            cpu->h = cpu->b;
            break;
        }

        case 0x41: //LD B, C
        {
            cpu->b = cpu->c;
            break;
        }

        case 0x51: //LD D, C
        {
            cpu->d = cpu->c;
            break;
        }

        case 0x61: //LD H, C
        {
            cpu->h = cpu->c;
            break;
        }

        case 0x02:// LD [BC], A
        {
            bus_write_byte(BUS, cpu_get_BC(cpu), cpu->a);
            cycle = 8;
            break;
        }

        case 0x12:// LD [DE], A
        {
            bus_write_byte(BUS, cpu_get_DE(cpu), cpu->a);
            cycle = 8;
            break;
        }

        case 0x42:// LD B, D
        {
            cpu->b = cpu->d;
            break;
        }

        case 0x52:// LD D, D
        {
            cpu->d = cpu->d;
            break;
        }

        case 0x62:// LD H, D
        {
            cpu->h = cpu->d;
            break;
        }

        case 0xF2: //LDH A, [C]
        {
            cpu->a = bus_read_byte(BUS, 0xFF00 + cpu->c);
            cycle = 8;
            break;
        }

        case 0x43: //LD B, E
        {
            cpu->b = cpu->e;
            break;
        }

        case 0x53: // LD D, E
        {
            cpu->d = cpu->e;
            break;
        }

        case 0x63: // LD H, E
        {
            cpu->h = cpu->e;
            break;
        }

        case 0x44: // LD B, H
        {
            cpu->b = cpu->h;
            break;
        }

        case 0x54: // LD D, H
        {
            cpu->d = cpu->h;
            break;
        }

        case 0x64: //LD H, H
        {
            cpu->h = cpu->h;
            break;
        }

        case 0x45: // LD B, L
        {
            cpu->b = cpu->l;
            break;
        }

        case 0x55: //LD D, L
        {
            cpu->d = cpu->l;
            break;
        }

        case 0x65: // LD H, L
        {
            cpu->h = cpu->l;
            break;
        }

        case 0x27: //DAA
        {
            uint8_t correction = 0;
            int carry = cpu_flag_is_set(cpu, FLAG_C);

            if (!cpu_flag_is_set(cpu, FLAG_N))
            {
                if (cpu_flag_is_set(cpu, FLAG_H) || (cpu->a & 0x0F) > 9)
                    correction |= 0x06;

                if (carry || cpu->a > 0x99)
                {
                    correction |= 0x60;
                    carry = 1;
                }

                cpu->a += correction;
            }
            else
            {
                if (cpu_flag_is_set(cpu, FLAG_H))
                    correction |= 0x06;

                if (carry)
                    correction |= 0x60;

                cpu->a -= correction;
            }

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, carry);

            break;
        }

        case 0x37: //SCF
        {
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 1);
            break;
        }

        case 0x48: // LD C, B
        {
            cpu->c = cpu->b;
            break;
        }

        case 0x58: // LD E, B
        {
            cpu->e = cpu->b;
            break;
        }

        case 0x68: // LD L, B
        {
            cpu->l = cpu->b;
            break;
        }

        case 0x49: //LD C, C
        {
            cpu->c = cpu->c;
            break;
        }

        case 0x59: //LD E, C
        {
            cpu->e = cpu->c;
            break;
        }

        case 0x69: //LD L, C
        {
            cpu->l = cpu->c;
            break;
        }

        case 0xE9: //JP HL
        {
            uint16_t address = ((uint16_t)cpu->h << 8) | cpu->l;
            cpu->pc = address;
            break;
        }

        case 0x0A: //LD A, [BC]
        {
            cpu->a = bus_read_byte(BUS, cpu_get_BC(cpu));
            cycle = 8;
            break;
        }

        case 0x1A: //LD A, [DE]
        {
            cpu->a = bus_read_byte(BUS, cpu_get_DE(cpu));
            cycle = 8;
            break;
        }

        case 0x4A: //LD C, D
        {
            cpu->c = cpu->d;
            break;
        }

        case 0x5A: //LD E, D
        {
            cpu->e = cpu->d;
            break;
        }

        case 0x6A: //LD L, D
        {
            cpu->l = cpu->d;
            break;
        }

        case 0x4B: // LD C, E
        {
            cpu->c = cpu->e;
            break;
        }

        case 0x5B: // LD E, E
        {
            cpu->e = cpu->e;
            break;
        }

        case 0x6B: //LD L, E
        {
            cpu->l = cpu->e;
            break;
        }

        case 0x4C: //LD C, H
        {
            cpu->c = cpu->h;
            break;
        }

        case 0x5C: //LD E, H
        {
            cpu->e = cpu->h;
            break;
        }

        case 0x6C: //LD L, H
        {
            cpu->l = cpu->h;
            break;
        }

        case 0x4D: //LD C, L
        {
            cpu->c = cpu->l;
            break;
        }

        case 0x5D: //LD E, L
        {
            cpu->e = cpu->l;
            break;
        }

        case 0x6D: //LD L, L
        {
            cpu->l = cpu->l;
            break;
        }

        case 0x2F: // CPL
        {
            cpu->a = ~cpu->a;
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            break;
        }

        case 0x3F: //CCF
        {
            int carry = cpu_flag_is_set(cpu, FLAG_C);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, !carry);
            break;
        }

        case 0x7F: //LD A, A
        {
            cpu->a = cpu->a;
            break;
        }

        case 0x3E: //LD A, d8
            uint8_t value = cpu_fetch_byte(cpu, BUS);
            cpu->a = value;
            cycle = 8;
            break;

        case 0x06: //LD B, d8
            value = cpu_fetch_byte(cpu, BUS);

            cpu->b = value;
            cycle = 8;
            break;

        case 0x0E:  //LD C, d8
            value = cpu_fetch_byte(cpu, BUS);
            cpu->c = value;
            cycle = 8;
            break;

        case 0x16:  //LD D, d8
            value = cpu_fetch_byte(cpu, BUS);
            cpu->d = value;
            cycle = 8;
            break;

        case 0x1E:  //LD E, d8
            value = cpu_fetch_byte(cpu, BUS);
            cpu->e = value;
            cycle = 8;
            break;

        case 0x26:  //LD H, d8
            value = cpu_fetch_byte(cpu, BUS);
            cpu->h = value;
            cycle = 8;
            break;

        case 0x2E:  //LD L,d8
            value = cpu_fetch_byte(cpu, BUS);
            cpu->l = value;
            cycle = 8;
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
            cycle = 8;
            break;

        case 0x77: //LD (HL), A // Writes A into byte at address stored in HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->a);
            cycle = 8;
            break;

        case 0x36: //LD (HL), d8 // Writes 8bit immediate value at address stored in HL
            value = cpu_fetch_byte(cpu, BUS);
            bus_write_byte(BUS, cpu_get_HL(cpu), value);
            cycle = 12;
            break;

        case 0x46: //LD B, (HL) // Loads byte at address HL into B
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->b = value;
            cycle = 8;
            break;

        case 0x4E: //LD C, (HL) // Loads byte at address HL into C
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->c = value;
            cycle = 8;

            break;

        case 0x56: //LD D, (HL) // Loads byte at address HL into D
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->d = value;
            cycle = 8;
            break;

        case 0x5E: //LD E, (HL) // Loads byte at address HL into E
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->e = value;
            cycle = 8;
            break;

        case 0x66: //LD H, (HL) // Loads byte at address HL into H
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->h = value;
            cycle = 8;
            break;

        case 0x6E: //LD L, (HL) // Loads byte at address HL into L
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            cpu->l = value;
            cycle = 8;
            break;

        case 0x70: //LD (HL), B // Writes B into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->b);
            cycle = 8;
            break;

        case 0x71: //LD (HL), C // Writes C into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->c);
            cycle = 8;
            break;

        case 0x72: //LD (HL), D // Writes D into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->d);
            cycle = 8;
            break;

        case 0x73: //LD (HL), E // Writes E into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->e);
            cycle = 8;
            break;

        case 0x74: //LD (HL), H // Writes H into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->h);
            cycle = 8;
            break;

        case 0x75: //LD (HL), L // Writes L into byte at address HL
            bus_write_byte(BUS, cpu_get_HL(cpu), cpu->l);
            cycle = 8;
            break;

        case 0x21: //LD HL, d16 // Load 16bit Little Endian immediate to HL
            uint8_t low = cpu_fetch_byte(cpu, BUS);
            uint8_t high = cpu_fetch_byte(cpu, BUS);
            uint16_t value_16 = ((uint16_t)high << 8 ) | low;
            cpu_set_HL(cpu, value_16);
            cycle = 12;
            break;

        case 0x01: //LD BC, d16 //Load 16bit Little Endian immediate to BC
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = ((uint16_t)high << 8 ) | low;
            cpu_set_BC(cpu, value_16);
            cycle = 12;
            break;

        case 0x11: //LD DE, d16 //Load 16bit Little Endian immediate to DE
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = ((uint16_t)high << 8 ) | low;
            cpu_set_DE(cpu, value_16);
            cycle = 12;
            break;

        case 0x31: //LD SP, d16 // Load 16bit Little Endian immediate to stack pointer
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = ((uint16_t)high << 8 ) | low;
            cpu->sp = value_16;
            cycle = 12;
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

        case 0xB9: //CP A,C // Compare A and C, record flags
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
            cycle = 16;
            break;

        case 0xC2: //JP NZ, d16 // Conditional jump to immediate 16bit address if ZERO Flag is cleared
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            cycle = 12;
            if (cpu_flag_is_set(cpu, FLAG_Z) == 0) {
                cycle = 16;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                break;
            } else {
                break;
            }

        case 0xCA: //JP Z, d16 // Conditional jump to immediate 16bit address if ZERO Flag is set
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            cycle = 12;
            if (cpu_flag_is_set(cpu, FLAG_Z) == 1) {

                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                cycle = 16;
                break;
            } else {
                break;
            }

        case 0xD2: //JP NC, d16 // Conditional jump to immediate 16bit address if CARRY Flag is cleared
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            cycle = 12;
            if (cpu_flag_is_set(cpu, FLAG_C) == 0) {
                cycle = 16;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                break;
            } else {
                break;
            }

        case 0xDA: //JP C, d16 // Conditional jump to immediate 16bit address if CARRY Flag is set
            cycle = 12;
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            if (cpu_flag_is_set(cpu, FLAG_C) == 1) {
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                cycle = 16;
                break;
            } else {
                break;
            }

        case 0x18: //JR, d8 // Unconditional jump to 8bit offset
            int8_t offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            cpu->pc = (uint16_t)(cpu->pc + offset);
            cycle = 12;
            break;

        case 0x20: //JR NZ,d8 // Conditional relative jump if ZERO FLAG is cleared
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            cycle = 8;
            if (cpu_flag_is_set(cpu, FLAG_Z) == 0) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
                cycle = 12;
            }
            break;

        case 0x28: //JR Z,d8 // Conditional relative jump if ZERO FLAG is set
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            cycle = 8;
            if (cpu_flag_is_set(cpu, FLAG_Z) == 1) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
                cycle = 12;
            }
            break;

        case 0x30: //JR NC,d8 // Conditional relative jump if CARRY FLAG is cleared
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            cycle = 8;
            if (cpu_flag_is_set(cpu, FLAG_C) == 0) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
                cycle = 12;
            }
            break;

        case 0x38: //JR C,d8 // Conditional relative jump if CARRY FLAG is set
            offset = (int8_t)cpu_fetch_byte(cpu, BUS);
            cycle = 8;
            if (cpu_flag_is_set(cpu, FLAG_C) == 1) {

                cpu->pc = (uint16_t)(cpu->pc + offset);
                cycle = 12;
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
            cycle = 24;
            break;

        case 0xC9: //RET //Returns from subroutine by reading the saved return address from the stack and loading it into the program counter
            low = bus_read_byte(BUS, cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS, cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu->pc = value_16;
            cycle = 16;

            break;

        case 0xC4: // //CALL NZ d16 // Calls subroutine when ZERO FLAG is cleared, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;
            cycle = 12;

            if (cpu_flag_is_set(cpu, FLAG_Z) == 0) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
                cycle = 24;
            }
            break;

        case 0xCC: // //CALL Z d16 // Calls subroutine when ZERO FLAG is set, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;
            cycle = 12;
            if (cpu_flag_is_set(cpu, FLAG_Z) == 1) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
                cycle = 24;
            }
            break;

        case 0xD4: // //CALL NC d16 // Calls subroutine when CARRY FLAG is cleared, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;
            cycle = 12;

            if (cpu_flag_is_set(cpu, FLAG_C) == 0) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
                cycle = 24;
            }
            break;

        case 0xDC: // //CALL C d16 // Calls subroutine when CARRY FLAG is set, CPU saves return address to stack pointer, then jumps to target address
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            value_16 = (uint16_t)high << 8 | low;
            cycle = 12;

            if (cpu_flag_is_set(cpu, FLAG_C) == 1) {
                return_address = cpu->pc;

                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
                cpu->sp--;
                bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
                cpu->pc = value_16;
                cycle = 24;
            }
            break;

        case 0xC5:// PUSH BC // Writes the 16bit value into the stack
            value_16 = cpu_get_BC(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            cycle = 16;
            break;

        case 0xD5:// PUSH DE // Writes the 16bit value into the stack
            value_16 = cpu_get_DE(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            cycle = 16;
            break;

        case 0xE5:// PUSH HL // Writes the 16bit value into the stack
            value_16 = cpu_get_HL(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            cycle = 16;
            break;

        case 0xF5:// PUSH AF // Writes the 16bit value into the stack
            value_16 = cpu_get_AF(cpu);
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(value_16 & 0xFF));
            cycle = 16;
            break;

        case 0xC1: //POP BC // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_BC(cpu,value_16);
            cycle = 12;
            break;

        case 0xD1: //POP DE // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_DE(cpu,value_16);
            cycle = 12;
            break;

        case 0xE1: //POP HL // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_HL(cpu,value_16);
            cycle = 12;
            break;

        case 0xF1: //POP AF // Reverses push, restores value from the stack
            low = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            high = bus_read_byte(BUS,cpu->sp);
            cpu->sp++;
            value_16 = (uint16_t)high << 8 | low;
            cpu_set_AF(cpu,value_16);
            cycle = 12;
            break;

        case 0xC0: //RET NZ //Return only if ZERO FLAG is cleared
            cycle = 8;
            if (cpu_flag_is_set(cpu,FLAG_Z) == 0) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                cycle = 20;
            }
            break;

        case 0xC8: //RET Z //Return only if ZERO FLAG is set
            cycle = 8;
            if (cpu_flag_is_set(cpu,FLAG_Z) == 1) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                cycle = 20;
            }
            break;

        case 0xD0: //RET NC //Return only if CARRY FLAG is cleared
            cycle = 8;
            if (cpu_flag_is_set(cpu,FLAG_C) == 0) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                cycle = 20;
            }
            break;

        case 0xD8: //RET C //Return only if ZERO FLAG is set
            cycle = 8;
            if (cpu_flag_is_set(cpu,FLAG_C) == 1) {
                low = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                high = bus_read_byte(BUS, cpu->sp);
                cpu->sp++;
                value_16 = (uint16_t)high << 8 | low;
                cpu->pc = value_16;
                cycle = 20;

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
            cycle = 16;
            break;

        case 0xC7: //RST 00H // Jumps to 0x0000 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0000;
            cycle = 16;
            break;

        case 0xCF: //RST 08H // Jumps to 0x0008 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0008;
            cycle = 16;
            break;

        case 0xD7: //RST 10H // Jumps to 0x0010 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0010;
            cycle = 16;
            break;

        case 0xDF: //RST 18H // Jumps to 0x0018 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0018;
            cycle = 16;
            break;

        case 0xE7: //RST 20H // Jumps to 0x0020 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0020;
            cycle = 16;
            break;

        case 0xEF: //RST 28H // Jumps to 0x0028 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0028;
            cycle = 16;
            break;

        case 0xF7: //RST 30H // Jumps to 0x0030 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0030;
            cycle = 16;
            break;

        case 0xFF: //RST 38H // Jumps to 0x0030 and pushes the return address to the stack
            return_address = cpu->pc;

            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address >> 8));
            cpu->sp--;
            bus_write_byte(BUS, cpu->sp,(uint8_t)(return_address & 0xFF));
            cpu->pc = 0x0038;
            cycle = 16;
            break;

        case 0x08: //LD (d16), SP writes to memory the stack pointer
            low = cpu_fetch_byte(cpu,BUS);
            high = cpu_fetch_byte(cpu,BUS);
            value_16 = ((uint16_t)high << 8) | low;
            uint16_t sp = cpu->sp;
            bus_write_byte(BUS,value_16, (uint8_t)(sp & 0xFF));
            bus_write_byte(BUS,value_16 + 1, (uint8_t)(sp >> 8));
            cycle = 20;
            break;

        case 0x03: //INC BC, Increment BC
            value_16 = cpu_get_BC(cpu);
            value_16++;
            cpu_set_BC(cpu,value_16);
            cycle = 8;
            break;

        case 0x13: //INC DE, Increment DE
            value_16 = cpu_get_DE(cpu);
            value_16++;
            cpu_set_DE(cpu,value_16);
            cycle = 8;
            break;

        case 0x23: //INC HL, Increment HL
            value_16 = cpu_get_HL(cpu);
            value_16++;
            cpu_set_HL(cpu,value_16);
            cycle = 8;
            break;

        case 0x33: //INC SP, Increment Stack Pointer
            value_16 = cpu->sp;
            value_16++;
            cpu->sp = value_16;
            cycle = 8;
            break;

        case 0x0B: //DEC BC, Decrement BC
            value_16 = cpu_get_BC(cpu);
            value_16--;
            cpu_set_BC(cpu,value_16);
            cycle = 8;
            break;

        case 0x1B: //DEC DE, Decrement DE
            value_16 = cpu_get_DE(cpu);
            value_16--;
            cpu_set_DE(cpu,value_16);
            cycle = 8;
            break;

        case 0x2B: //DEC HL, Decrement HL
            value_16 = cpu_get_HL(cpu);
            value_16--;
            cpu_set_HL(cpu,value_16);
            cycle = 8;
            break;

        case 0x3B: //DEC SP, Decrement Stack Pointer
            value_16 = cpu->sp;
            value_16--;
            cpu->sp = value_16;
            cycle = 8;
            break;

        case 0x09: // ADD HL,BC //add BC to HL, store in HL
            uint16_t hl = cpu_get_HL(cpu);
            uint16_t bc = cpu_get_BC(cpu);
            uint32_t result_32 = (uint32_t)hl + bc;
            cpu_set_HL(cpu,(uint16_t)result_32);

            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((hl & 0x0FFF) + (bc & 0x0FFF)) > 0x0FFF);
            cpu_set_flag(cpu, FLAG_C, result_32 > 0xFFFF);
            cycle = 8;
            break;

        case 0x19: // ADD HL,DE //add DE to HL, store in HL
            hl = cpu_get_HL(cpu);
            uint16_t de = cpu_get_DE(cpu);
            result_32 = (uint32_t)hl + de;
            cpu_set_HL(cpu,(uint16_t)result_32);

            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((hl & 0x0FFF) + (de & 0x0FFF)) > 0x0FFF);
            cpu_set_flag(cpu, FLAG_C, result_32 > 0xFFFF);
            cycle = 8;
            break;

        case 0x29: // ADD HL,HL //add HL to HL, store in HL
            hl = cpu_get_HL(cpu);

            result_32 = (uint32_t)hl + hl;
            cpu_set_HL(cpu,(uint16_t)result_32);

            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((hl & 0x0FFF) + (hl & 0x0FFF)) > 0x0FFF);
            cpu_set_flag(cpu, FLAG_C, result_32 > 0xFFFF);
            cycle = 8;
            break;

        case 0x39: // ADD HL,SP //add the Stack Pointer to HL, store in HL
            hl = cpu_get_HL(cpu);
            sp = cpu->sp;
            result_32 = (uint32_t)hl + sp;
            cpu_set_HL(cpu,(uint16_t)result_32);

            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((hl & 0x0FFF) + (sp                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           & 0x0FFF)) > 0x0FFF);
            cpu_set_flag(cpu, FLAG_C, result_32 > 0xFFFF);
            cycle = 8;
            break;

        case 0xE8: //ADD SP, d8// add signed 8bit offset to SP, store in SP
            value = cpu_fetch_byte(cpu, BUS);
            offset = (int8_t)value;
            sp = cpu->sp;
            cpu->sp = (uint16_t)(sp + offset);

            cpu_set_flag(cpu, FLAG_Z, 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((sp & 0x0F) + (value & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, ((sp & 0xFF) + value) > 0xFF);
            cycle = 16;

            break;

        case 0xF8: //LD HL, SP+d8 // sum sp and 8bit s_offset, store in HL
            value = cpu_fetch_byte(cpu, BUS);
            offset = (int8_t)value;
            sp = cpu->sp;
            cpu_set_HL(cpu,(uint16_t)(sp + offset));

            cpu_set_flag(cpu, FLAG_Z, 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((sp & 0x0F) + (value & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, ((sp & 0xFF) + value) > 0xFF);
            cycle = 12;
            break;

        case 0xF9: //LD SP,HL // LOAD HL into the Stack Pointer
            value_16 = cpu_get_HL(cpu);
            cpu->sp = value_16;
            cycle = 8;
            break;

        case 0xE0: //LDH (a8), A writes A into HRAM
            value = cpu_fetch_byte(cpu, BUS);
            uint16_t address = 0xFF00 + value;
            a = cpu->a;
            bus_write_byte(BUS, address, a);
            cycle = 12;
            break;

        case 0xF0: //LDH A, (a8) //Writes HRAM byte into A
            value = cpu_fetch_byte(cpu, BUS);
            address = 0xFF00 + value;
            a = bus_read_byte(BUS, address);
            cpu->a = a;
            cycle = 12;
            break;

        case 0xE2: //LD (C), A //writes register A to the address 0xFF00 + C, using register C as the 8-bit offset.
            address = 0xFF00 + cpu->c;
            a = cpu->a;
            bus_write_byte(BUS, address, a);
            cycle = 8;
            break;

        case 0xEA: //LD (a16), A//write register A to a 16-bit memory address.
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            address = ((uint16_t)high << 8) | low;
            a = cpu->a;
            bus_write_byte(BUS, address, a);
            cycle = 16;
            break;

        case 0xFA://LD A, (a16) // Load byte at address into A
        {
            low = cpu_fetch_byte(cpu, BUS);
            high = cpu_fetch_byte(cpu, BUS);
            address = ((uint16_t)high << 8) | low;
            cpu->a = bus_read_byte(BUS, address);
            cycle = 16;
            break;
        }
        case 0x32: //LD (HL-), A // writes A into byte at the address in HL, then decrements HL
        {
            address = cpu_get_HL(cpu);
            a = cpu->a;
            bus_write_byte(BUS, address, a);
            cpu_set_HL(cpu, address - 1);
            cycle = 8;
            break;
        }
        case 0x3A: //LD A, (HL-) // writes byte at address HL into A, then decrements HL
        {
            address = cpu_get_HL(cpu);
            cpu->a = bus_read_byte(BUS, address);
            cpu_set_HL(cpu, address - 1);
            cycle = 8;
            break;
        }
        case 0x22: //LD (HL+), A // writes A into byte at the address in HL, then increments HL
        {
            address = cpu_get_HL(cpu);
            a = cpu->a;
            bus_write_byte(BUS, address, a);
            cpu_set_HL(cpu, address + 1);
            cycle = 8;
            break;
        }
        case 0x2A: //LD A, (HL+) // writes byte at address HL into A, then increments HL
        {
            address = cpu_get_HL(cpu);
            cpu->a = bus_read_byte(BUS, address);
            cpu_set_HL(cpu, address + 1);
            cycle = 8;
            break;
        }
        case 0x14: //INC D
        {
            result = cpu->d + 1;
            cpu->d = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);

            break;
        }

        case 0x15: //DEC D
        {
            value = cpu->d;
            result = cpu->d - 1;
            cpu->d = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (value & 0x0F) == 0);

            break;
        }

        case 0x3C: //INC A
        {
            result = cpu->a + 1;
            cpu->a = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);

            break;
        }

        case 0x3D: //DEC A
        {
            value = cpu->a;
            result = cpu->a - 1;
            cpu->a = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (value & 0x0F) == 0);

            break;
        }

        case 0x1C: //INC E
        {
            result = cpu->e + 1;
            cpu->e = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);

            break;
        }

        case 0x1D: //DEC E
        {
            value = cpu->e;
            result = cpu->e - 1;
            cpu->e = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (value & 0x0F) == 0);

            break;
        }

        case 0x24: //INC H
        {
            result = cpu->h + 1;
            cpu->h = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);

            break;
        }

        case 0x25: //DEC H
        {
            value = cpu->h;
            result = cpu->h - 1;
            cpu->h = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (value & 0x0F) == 0);

            break;
        }

        case 0x2C: //INC L
        {
            result = cpu->l + 1;
            cpu->l = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);

            break;
        }

        case 0x2D: //DEC L
        {
            value = cpu->l;
            result = cpu->l - 1;
            cpu->l = result;
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (value & 0x0F) == 0);

            break;
        }

        case 0x34: //INC (HL)
        {
            value_16 = cpu_get_HL(cpu);
            value = bus_read_byte(BUS, value_16);
            result = value + 1;

            bus_write_byte(BUS, value_16, result);
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, (result & 0x0F) == 0);
            cycle = 12;

            break;
        }

        case 0x35: //DEC (HL)
        {
            value_16 = cpu_get_HL(cpu);
            value = bus_read_byte(BUS, value_16);
            result = value - 1;

            bus_write_byte(BUS, value_16, result);
            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (value & 0x0F) == 0);
            cycle = 12;
            break;
        }

        case 0x88: //ADC A, B// ADD A and B with carry , store in A
        {
            a = cpu->a;
            b = cpu->b;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + b + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (b & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x89://ADC A, C// ADD A and C with carry , store in A
        {
            a = cpu->a;
            c = cpu->c;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + c + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (c & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x98: //SBC A,B SUB A and B with carry
        {
            a = cpu->a;
            b = cpu->b;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 1: %d\n", carry);
            int16_t sresult_16 = (int16_t)a - b - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((b & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            break;

        }

        case 0x99:// SBC A,C
            {
                a = cpu->a;
                c = cpu->c;
                uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
                //printf("carry 2: %d\n", carry);
               int16_t sresult_16 = (int16_t)a - c - carry;
                result = (uint8_t)sresult_16;
                cpu->a = result;

                cpu_set_flag(cpu, FLAG_Z, result == 0);
                cpu_set_flag(cpu, FLAG_N, 1);
                cpu_set_flag(cpu, FLAG_H,
                             (a & 0x0F) < ((c & 0x0F) + carry));
                cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
                break;
            }

        case 0x9A:// SBC A,D
        {
            a = cpu->a;
            uint8_t d = cpu->d;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
            int16_t sresult_16 = (int16_t)a - d - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((d & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            break;
        }

        case 0x9B:// SBC A,E
        {
            a = cpu->a;
            uint8_t e = cpu->e;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
            int16_t sresult_16 = (int16_t)a - e - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((e & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            break;
        }

        case 0x9C:// SBC A,H
        {
            a = cpu->a;
            uint8_t h = cpu->h;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
           int16_t sresult_16 = (int16_t)a - h - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((h & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            break;
        }

        case 0x9D:// SBC A,L
        {
            a = cpu->a;
            uint8_t l = cpu->l;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
            int16_t sresult_16 = (int16_t)a - l - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((l & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            break;
        }

        case 0x9E:// SBC A, (HL)
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
           int16_t sresult_16 = (int16_t)a - value - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((value & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            cycle = 8;
            break;
        }

        case 0x9F:// SBC A,A
        {
            a = cpu->a;

            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
            int16_t sresult_16 = (int16_t)a - a - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((a & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            break;
        }

        case 0x8A: //ADC A, D// ADD A and D with carry , store in A
        {
            a = cpu->a;
            uint8_t d = cpu->d;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + d + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (d & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x8B: //ADC A, E// ADD A and E with carry , store in A
        {
            a = cpu->a;
            uint8_t e = cpu->e;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + e + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (e & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x8C: //ADC A, H// ADD A and H with carry , store in A
        {
            a = cpu->a;
            uint8_t h = cpu->h;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + h + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (h & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x8D: //ADC A, L// ADD A and L with carry , store in A
        {
            a = cpu->a;
            uint8_t l = cpu->l;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + l + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (l & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x8E: //ADC A, (HL)// ADD A and (HL) with carry , store in A
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu)); ;
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + value + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (value & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            cycle = 8;

            break;
        }

        case 0x8F: //ADC A, A// ADD A and A with carry , store in A
        {
            a = cpu->a;

            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + a + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (a & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);

            break;
        }

        case 0x82: //ADD A,D
        {
            a = cpu->a;
            uint8_t d = cpu->d;

            result_16 = a + d;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (d & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;
        }

        case 0x83: //ADD A,E
        {
            a = cpu->a;
            uint8_t e = cpu->e;

            result_16 = a + e;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (e & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;
        }

        case 0x84: //ADD A,H
        {
            a = cpu->a;
            uint8_t h = cpu->h;

            result_16 = a + h;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (h & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;
        }

        case 0x85: //ADD A,L
        {
            a = cpu->a;
            uint8_t l = cpu->l;

            result_16 = a + l;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (l & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;
        }

        case 0x86: //ADD A,(HL)
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));

            result_16 = a + value;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (value & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            cycle = 8;
            break;
        }

        case 0x87: //ADD A, A
        {
            a = cpu->a;

            result_16 = a + a;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (a & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            break;
        }

        case 0x92: //SUB A, D // Sub D from A, store in A, record all flags
        {
            a = cpu->a;
            uint8_t d = cpu->d;

            result_16 = a - d;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (d & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < d);
            break;
        }

        case 0x93: //SUB A, E // Sub E from A, store in A, record all flags
        {
            a = cpu->a;
            uint8_t e = cpu->e;

            result_16 = a - e;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (e & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < e);
            break;
        }

        case 0x94: //SUB A, H // Sub H from A, store in A, record all flags
        {
            a = cpu->a;
            uint8_t h = cpu->h;

            result_16 = a - h;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (h & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < h);
            break;
        }

        case 0x95: //SUB A, L // Sub L from A, store in A, record all flags
        {
            a = cpu->a;
            uint8_t l = cpu->l;

            result_16 = a - l;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (l & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < l);
            break;
        }

        case 0x96: //SUB A, (HL) // Sub (HL) from A, store in A, record all flags
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));

            result_16 = a - value;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (value & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < value);
            cycle = 8;
            break;
        }

        case 0x97: //SUB A, A // Sub A from A, store in A, record all flags
        {
            a = cpu->a;

            result_16 = a - a;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xA2: //AND A,D
        {
            a = cpu->a;
            uint8_t d = cpu->d;
            result = a & d;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xA3: //AND A,E
        {
            a = cpu->a;
            uint8_t e = cpu->e;
            result = a & e;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xA4: //AND A,H
        {
            a = cpu->a;
            uint8_t h = cpu->h;
            result = a & h;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xA5: //AND A,L
        {
            a = cpu->a;
            uint8_t l = cpu->l;
            result = a & l;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xA6: //AND A,(HL)
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            result = a & value;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            cycle = 8;
            break;
        }

        case 0xA7: //AND A,A
        {
            a = cpu->a;

            result = a & a;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xB2: // OR A, D //Bitwise OR between A and D, stores in A, record flags
        {
            a = cpu->a;
            uint8_t d = cpu->d;
            result = a | d;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xB3: // OR A, E //Bitwise OR between A and E, stores in A, record flags
        {
            a = cpu->a;
            uint8_t e = cpu->e;
            result = a | e;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xB4: // OR A, H //Bitwise OR between A and H, stores in A, record flags
        {
            a = cpu->a;
            uint8_t h = cpu->h;
            result = a | h;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xB5: // OR A, L //Bitwise OR between A and L, stores in A, record flags
        {
            a = cpu->a;
            uint8_t l = cpu->l;
            result = a | l;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xB6: // OR A, (HL) //Bitwise OR between A and (HL), stores in A, record flags
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            result = a | value;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            cycle = 8;
            break;
        }

        case 0xB7: // OR A, A //Bitwise OR between A and A, stores in A, record flags
        {
            a = cpu->a;
            result = a | a;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xAA: // XOR A, D //Bitwise XOR between A and D, stores in A, record flags
        {
            a = cpu->a;
            uint8_t d = cpu->d;
            result = a ^ d;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xAB: // XOR A, E //Bitwise XOR between A and E, stores in A, record flags
        {
            a = cpu->a;
            uint8_t e = cpu->e;
            result = a ^ e;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xAC: // XOR A, H //Bitwise XOR between A and H, stores in A, record flags
        {
            a = cpu->a;
            uint8_t h = cpu->h;
            result = a ^ h;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xAD: // XOR A, L //Bitwise XOR between A and L, stores in A, record flags
        {
            a = cpu->a;
            uint8_t l = cpu->l;
            result = a ^ l;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xAE: // XOR A, (HL) //Bitwise XOR between A and (HL), stores in A, record flags
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));
            result = a ^ value;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            cycle = 8;
            break;
        }

        case 0xAF: // XOR A, A //Bitwise XOR between A and A, stores in A, record flags
        {
            a = cpu->a;
            result = 0;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xBA: //CP A,D // Compare A and D, record flags
        {
            a = cpu->a;
            uint8_t d = cpu->d;

            cpu_set_flag(cpu, FLAG_Z, a == d);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (d & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < d);
            break;
        }

        case 0xBB: //CP A,E // Compare A and E, record flags
        {
            a = cpu->a;
            uint8_t e = cpu->e;

            cpu_set_flag(cpu, FLAG_Z, a == e);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (e & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < e);
            break;
        }

        case 0xBC: //CP A,H // Compare A and H, record flags
        {
            a = cpu->a;
            uint8_t h = cpu->h;

            cpu_set_flag(cpu, FLAG_Z, a == h);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (h & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < h);
            break;
        }

        case 0xBD: //CP A,L // Compare A and L, record flags
        {
            a = cpu->a;
            uint8_t l = cpu->l;

            cpu_set_flag(cpu, FLAG_Z, a == l);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (l & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < l);
            break;
        }

        case 0xBE: //CP A,(HL) // Compare A and (HL), record flags
        {
            a = cpu->a;
            value = bus_read_byte(BUS, cpu_get_HL(cpu));

            cpu_set_flag(cpu, FLAG_Z, a == value);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (value & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < value);
            cycle = 8;
            break;
        }

        case 0xBF: //CP A,A // Compare A and A, record flags
        {
            a = cpu->a;

            cpu_set_flag(cpu, FLAG_Z, 1);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            break;
        }

        case 0xC6: //ADD A, d8 // Add B to A, store in A, record all flags
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);

            result_16 = a + value;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) + (value & 0x0F)) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            cycle = 8;
            break;
        }

        case 0xCE: //ADC A, d8// ADD A and d8 with carry , store in A
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            result_16 = (uint16_t)a + value + carry;
            cpu->a = (uint8_t)result_16;

            cpu_set_flag(cpu, FLAG_Z, cpu->a == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H,((a & 0x0F) + (value & 0x0F) + carry) > 0x0F);
            cpu_set_flag(cpu, FLAG_C, result_16 > 0xFF);
            cycle = 8;

            break;
        }

        case 0xD6: //SUB A, d8 // Sub d8 from A, store in A, record all flags
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);

            result_16 = a - value;
            cpu->a = (uint8_t)result_16;
            cpu_set_flag(cpu, FLAG_Z, result_16 == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, ((a & 0x0F) < (value & 0x0F)));
            cpu_set_flag(cpu, FLAG_C, a < value);
            cycle = 8;
            break;
        }

        case 0xDE:// SBC A,d8
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);
            uint8_t carry = cpu_flag_is_set(cpu, FLAG_C);
            //printf("carry 2: %d\n", carry);
            int16_t sresult_16 = (int16_t)a - value - carry;
            result = (uint8_t)sresult_16;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H,
                         (a & 0x0F) < ((value & 0x0F) + carry));
            cpu_set_flag(cpu, FLAG_C, sresult_16 < 0);
            cycle = 8;
            break;
        }

        case 0xE6: //AND A,d8
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);
            result = a & value;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 1);
            cpu_set_flag(cpu, FLAG_C, 0);
            cycle = 8;
            break;
        }

        case 0xEE: // XOR A, d8 //Bitwise XOR between A and d8, stores in A, record flags
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);
            result = a ^ value;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            cycle = 8;
            break;
        }

        case 0xF6: // OR A, d8 //Bitwise OR between A and d8, stores in A, record flags
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);
            result = a | value;
            cpu->a = result;

            cpu_set_flag(cpu, FLAG_Z, result == 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, 0);
            cycle = 8;
            break;
        }

        case 0xFE: //CP A,d8 // Compare A and d8, record flags
        {
            a = cpu->a;
            value = cpu_fetch_byte(cpu, BUS);

            cpu_set_flag(cpu, FLAG_Z, a == value);
            cpu_set_flag(cpu, FLAG_N, 1);
            cpu_set_flag(cpu, FLAG_H, (a & 0x0F) < (value & 0x0F));
            cpu_set_flag(cpu, FLAG_C, a < value);
            cycle = 8;
            break;
        }

        case 0x07: //RLCA//Rotate Left Circular Accumulator
        {
            a = cpu->a;
           uint8_t carry = (a & 0x80) != 0;
            a = (uint8_t)((a << 1) | carry);
            cpu->a = a;
            cpu_set_flag(cpu, FLAG_Z, 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, carry);
            break;
        }

        case 0x0F: //RRCA//Rotate Right Circular Accumulator
        {
            a = cpu->a;
            uint8_t carry = (a & 0x01) != 0;
            a = (uint8_t)((a >> 1) | (carry << 7));
            cpu->a = a;
            cpu_set_flag(cpu, FLAG_Z, 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, carry);
            break;
        }

        case 0x17: //RLA//Rotate Left through Carry, Accumulator
        {
            a = cpu->a;
            uint8_t old_carry = cpu_flag_is_set(cpu, FLAG_C);
            uint8_t new_carry = (a & 0x80) != 0;
            a = (uint8_t)((a << 1) | old_carry);
            cpu->a = a;
            cpu_set_flag(cpu, FLAG_Z, 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, new_carry);
            break;
        }

        case 0x1F: //RRA//Rotate Right through Carry, Accumulator
        {
            a = cpu->a;
            uint8_t new_carry = (a & 0x01) != 0;
            uint8_t old_carry = cpu_flag_is_set(cpu, FLAG_C);
            a = (uint8_t)((a >> 1) | (old_carry << 7));
            cpu->a = a;
            cpu_set_flag(cpu, FLAG_Z, 0);
            cpu_set_flag(cpu, FLAG_N, 0);
            cpu_set_flag(cpu, FLAG_H, 0);
            cpu_set_flag(cpu, FLAG_C, new_carry);
            break;
        }
        case 0xCB: //Prefix instructions
        {
            uint8_t cb_opcode = cpu_fetch_byte(cpu, BUS);
            cpu_execute_cb(cpu, BUS, cb_opcode);


            uint8_t target = cb_opcode & 0x07;
            uint8_t group = cb_opcode >> 6;

            if (target == 6) {
                cycle = (group == 1) ? 12 : 16;
            } else {
                cycle = 8;
            }


            break;
        }

        case 0xF3: //DI
        {
            cpu->ime = 0;
            cpu->ime_scheduled = 0;
            break;
        }

        case 0xFB: //EI
        {
            cpu->ime_scheduled = 1;
            break;
        }

        case 0x76: //HALT
        {
            uint8_t ie = bus_read_byte(BUS, 0xFFFF);
            uint8_t flags = bus_read_byte(BUS, 0xFF0F);
            uint8_t pending = ie & flags & 0x1F;

            if (!cpu->ime && pending != 0) {
                cpu->halt_bug = 1;
            } else {
                cpu->halted = 1;
            }
            break;
        }


        default:
            printf("Unknown opcode: 0x%02X at PC: %04X\n", opcode, cpu->pc - 1);
            break;
    }



    cpu->cycles += cycle;


    (void)opcode;
}


