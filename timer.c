//
// Created by Livia on 05.11.2026.
//


#include "timer.h"

#include <string.h>

#include "BUS.h"

void timer_init(TIMER *timer) {
    memset(timer, 0, sizeof(TIMER));
}

uint8_t timer_read_register(TIMER *timer, uint16_t address) {
    switch (address) {

        //Divider counter
        case 0xFF04:
            return (uint8_t)(timer->divider >> 8);

            //TIMA Timer counter
        case 0xFF05:
            return timer->tima;

        //TMA Timer modulo/reload val
        case 0xFF06:
            return timer->tma;

            //Timer Control
        case 0xFF07:
            return timer->tac | 0xF8;

        default:
            return 0xFF;
    }
}

void timer_write_register(TIMER *timer, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xFF04: // added glitch
        {
            int old_signal = timer_signal(timer);
            timer->divider = 0;
            int new_signal = timer_signal(timer);

            if (old_signal && !new_signal && timer->overflow_delay == 0) {
                timer->tima++;
                if (timer->tima == 0) {
                    timer->overflow_delay = 4;
                }
            }

            break;
        }

        case 0xFF05:
            if (timer->overflow_delay == 0) {
                timer->tima = value;
            }
            break;

        case 0xFF06:
            timer->tma = value;

            if (timer->overflow_delay == 1) {
                timer->tima = value;
            }
            break;

        case 0xFF07:
        {
            int old_signal = timer_signal(timer);
            timer->tac = value & 0x07;
            int new_signal = timer_signal(timer);
            if (old_signal && !new_signal && timer->overflow_delay == 0) {
                timer->tima++;

                if (timer->tima == 0) {
                    timer->overflow_delay = 4;
                }
            }
            break;
        }

        default:
            break;
    }
}

static int timer_signal(TIMER *timer) {
    if (!(timer->tac & 0x04)) {
        return 0;
    }

    static const int bits[4] = {9, 3, 5, 7};

    int selected_bit = bits[timer->tac & 0x03];

    return (timer->divider >> selected_bit) & 1;
}


void timer_update(TIMER *timer, uint64_t cycles, uint8_t *interrupt_flags) {

    for (uint64_t i = 0; i < cycles; i++) {
        //Handle pending TIMA reload
        if (timer->overflow_delay > 0) {
            timer->overflow_delay--;
            if (timer->overflow_delay == 0) {
                timer->tima = timer->tma;
                *interrupt_flags |= 0x04;
            }
        }
        int old_signal = timer_signal(timer);

        timer->divider++;

        int new_signal = timer_signal(timer);

        //TIMA increments on falling edge
        if (old_signal && !new_signal && timer->overflow_delay == 0) {
            timer->tima++;

            if (timer->tima == 0) {
                timer->overflow_delay = 4;
            }
        }
    }
}