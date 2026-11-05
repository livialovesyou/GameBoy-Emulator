//
// Created by Livia on 05.11.2026.
//

#pragma once

#include <stdint.h>

typedef struct TIMER {
    uint16_t divider;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;
    uint8_t overflow_delay;
} TIMER;

void timer_init(TIMER *timer);
static int timer_signal(TIMER *timer);
void timer_update(TIMER *timer, uint64_t cycles, uint8_t *interrupt_flags);
uint8_t timer_read_register(TIMER *timer, uint16_t address);
void timer_write_register(TIMER *timer, uint16_t address, uint8_t value);
