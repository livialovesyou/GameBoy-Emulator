//
// Created by Livia on 23.10.2026.
//

#pragma once

#include <stdint.h>
#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

typedef struct BUS BUS;
typedef struct {
        uint8_t a;      //Accumulator, commonly used in arithmetic
        uint8_t f;      //flags register
        uint8_t b;      //General-purpose storage
        uint8_t c;      //General-purpose storage
        uint8_t d;      //General-purpose storage
        uint8_t e;      //General-purpose storage
        uint8_t h;      //General-purpose storage
        uint8_t l;      //General-purpose storage

        uint16_t sp;    //Stack Pointer
        uint16_t pc;    //Program Counter
        uint8_t ime;    //Interrupt Master Enable
        uint8_t ime_scheduled;
        uint64_t cycles;
        uint8_t halted; //Halt
        uint8_t halt_bug;

        uint8_t stopped; //STOP


} CPU;

//Read/Write BC
static inline uint16_t cpu_get_BC(const CPU *cpu) {
        return((uint16_t)cpu->b << 8 ) | (uint16_t)cpu->c;
}
static inline void cpu_set_BC(CPU *cpu, uint16_t value) {
        cpu->b = (uint8_t)(value >> 8) & 0xFF;
        cpu->c = (uint8_t)(value & 0xFF);
}

//Read/Write DE
static inline uint16_t cpu_get_DE(const CPU *cpu) {
        return((uint16_t)cpu->d << 8 ) | (uint16_t)cpu->e;
}
static inline void cpu_set_DE(CPU *cpu, uint16_t value) {
        cpu->d = (uint8_t)(value >> 8) & 0xFF;
        cpu->e = (uint8_t)(value & 0xFF);
}

//Read/Write HL
static inline uint16_t cpu_get_HL(const CPU *cpu) {
        return ((uint16_t)cpu->h << 8 ) | cpu->l;

}
static inline void cpu_set_HL(CPU *cpu, uint16_t value) {
        cpu->h = (uint8_t)(value >> 8) & 0xFF;
        cpu->l = (uint8_t)(value & 0xFF);
}

//check/Set flag for register F
static inline int cpu_flag_is_set(const CPU *cpu, uint8_t flag) {
        return (cpu->f & flag) != 0;
}

static inline void cpu_set_flag(CPU *cpu, uint8_t flag, uint8_t set) {
        if (set) {
                cpu->f |= flag;
        } else {
                cpu->f &= ~flag;
        }
        cpu->f &= 0xF0;
}

//Read/Write AF
static inline uint16_t cpu_get_AF(const CPU *cpu) {
        return((uint16_t)cpu->a << 8 ) | (uint16_t)(cpu->f & 0xF0);
}
static inline void cpu_set_AF(CPU *cpu, uint16_t value) {
        cpu->a = (uint8_t)(value >> 8) & 0xFF;
        cpu->f = (uint8_t)(value & 0xF0);
}

void cpu_step(CPU *cpu, BUS *BUS);

static uint8_t cpu_cb_read_target(CPU *cpu, BUS *BUS, uint8_t target);

static void cpu_cb_write_target(CPU *cpu, BUS *BUS, uint8_t target, uint8_t value);

void cpu_execute_cb(CPU *cpu, BUS *BUS, uint8_t opcode);

static int cpu_service_interrupt(CPU *cpu, BUS *BUS);