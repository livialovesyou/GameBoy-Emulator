//
// Created by Livia on 23.10.2026.
//

#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"

void echo_ram_test_1(void) {
    BUS BUS = {0};
    bus_write_byte(&BUS, 0xC002, 42);
    uint8_t result = bus_read_byte(&BUS, 0xE002 );
    if (result == 42) {
        printf("echo_ram_test_1(read): success\n");
    } else {
        printf("echo_ram_test_1(read): fail\n");
    }
    bus_write_byte(&BUS, 0xE002, 41);
    result = bus_read_byte(&BUS, 0xC002 );
    if (result == 41) {
        printf("echo_ram_test1(write): success\n");
    } else {
        printf("echo_ram_test1(write): fail\n");
    }
}

void test_unimplemented_address(void)
{
    BUS BUS = {0};

    if (bus_read_byte(&BUS, 0x8000) == 0xFF) {
        printf("Unimplemented address test passed\n");
    } else {
        printf("Unimplemented address test failed\n");
    }
}

void test_cpu_registers_1(void) {
    BUS BUS = {0};
    CPU cpu = {0};

    // cpu.a = 0x42;
    // cpu.f = FLAG_Z;
    cpu_set_AF(&cpu, 0x4280);
    cpu_set_BC(&cpu, 0xABCD);
    cpu_set_DE(&cpu, 0xDEAD);
    cpu_set_HL(&cpu, 0xDEAD);

    if (cpu.a == 0x42) {
        printf("CPU Registers A test passed\n");
    }
    if (cpu_get_AF(&cpu) == 0x4280) {
        printf("CPU Registers AF test passed\n");
    }

    if (cpu_get_BC(&cpu) == 0xABCD) {
        printf("CPU_Registers BC test passed\n");
    }
    if (cpu_get_DE(&cpu) == 0xDEAD) {
        printf("CPU_Registers DE test passed\n");
    }
    if (cpu_get_HL(&cpu) == 0xDEAD) {
        printf("CPU_Registers HL test passed\n");
    }
}

void test_cpu_flags_1(void) {
    BUS BUS = {0};
    CPU cpu = {0};
    cpu.f = FLAG_Z | FLAG_C;
    if (cpu_flag_is_set(&cpu, FLAG_Z) != 0) {
        printf("FLAG_Z test passed\n");
    }
    if (cpu_flag_is_set(&cpu, FLAG_C) != 0) {
        printf("FLAG_C test passed\n");
    }

    cpu_set_flag(&cpu, FLAG_N, 1);
    cpu_set_flag(&cpu, FLAG_H, 1);
    if (cpu_flag_is_set(&cpu, FLAG_N) != 0) {
        printf("FLAG_N test passed\n");
    }
    if (cpu_flag_is_set(&cpu, FLAG_H) != 0) {
        printf("FLAG_H test passed\n");
    }
}

void test_cpu_opcodes_1(void) {
    BUS BUS = {0};
    CPU cpu = {0};

    cpu.pc = 0xC000;
    bus_write_byte(&BUS, cpu.pc, 0x3E);                 //LD A,d8
    bus_write_byte(&BUS, cpu.pc + 1, 0x42);
    bus_write_byte(&BUS, cpu.pc + 2, 0x06);     //LD B,d8
    bus_write_byte(&BUS, cpu.pc + 3, 0x43);
    bus_write_byte(&BUS, cpu.pc + 4, 0x0E);     //LD C,d8
    bus_write_byte(&BUS, cpu.pc + 5, 0x44);
    bus_write_byte(&BUS, cpu.pc + 6, 0x16);     //LD D,d8
    bus_write_byte(&BUS, cpu.pc + 7, 0x45);
    bus_write_byte(&BUS, cpu.pc + 8, 0x1E);     //LD E,d8
    bus_write_byte(&BUS, cpu.pc + 9, 0x46);
    bus_write_byte(&BUS, cpu.pc + 10, 0x26);    //LD H,d8
    bus_write_byte(&BUS, cpu.pc + 11, 0x47);
    bus_write_byte(&BUS, cpu.pc + 12, 0x2E);    //LD L,d8
    bus_write_byte(&BUS, cpu.pc + 13, 0x48);

    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42) {
        printf("CPU opcodes test LD A,d8 passed\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.b == 0x43) {
        printf("CPU opcodes test LD B,d8 passed\n");
    }


    cpu_step(&cpu, &BUS);
    if (cpu.c == 0x44) {
        printf("CPU opcodes test LD C,d8 passed\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.d == 0x45) {
        printf("CPU opcodes test LD D,d8 passed\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.e == 0x46) {
        printf("CPU opcodes test LD E,d8 passed\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.h == 0x47) {
        printf("CPU opcodes test LD H,d8 passed\n");
    }
    cpu_step(&cpu, &BUS);
    if (cpu.l == 0x48) {
        printf("CPU opcodes test LD L,d8 passed\n");
    }
}

void test_cpu_opcodes_2(void) {
    BUS BUS = {0};
    CPU cpu = {0};
    cpu.pc = 0xC000;
    cpu.a = 0x12;
    cpu.b = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x47); //LD B, A
    bus_write_byte(&BUS, cpu.pc + 1, 0x4F); //LD C, A
    bus_write_byte(&BUS, cpu.pc + 2, 0x57); //LD D, A
    bus_write_byte(&BUS, cpu.pc + 3, 0x5F); //LD E, A
    bus_write_byte(&BUS, cpu.pc + 4, 0x67); //LD H, A
    bus_write_byte(&BUS, cpu.pc + 5, 0x6F); //LD L, A
    bus_write_byte(&BUS, cpu.pc + 6, 0x78); //LD A, B
    bus_write_byte(&BUS, cpu.pc + 7, 0x79); //LD A, C
    bus_write_byte(&BUS, cpu.pc + 8, 0x7A); //LD A, D
    bus_write_byte(&BUS, cpu.pc + 9, 0x7B); //LD A, E
    bus_write_byte(&BUS, cpu.pc + 10,0x7C); //LD A, H
    bus_write_byte(&BUS, cpu.pc + 11,0x7D); //LD A, L
    cpu_step(&cpu, &BUS);
    if (cpu.b == 0x12) {
        printf("CPU opcodes test LD B,A passed\n");
    } else {
        printf("CPU opcodes test LD B,A FAIL\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.c == 0x12) {
        printf("CPU opcodes test LD C,A passed\n");
    }   else {
        printf("CPU opcodes test LD C,A FAIL\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.d == 0x12) {
        printf("CPU opcodes test LD D,A passed\n");
    } else {
        printf("CPU opcodes test LD D,A FAIL\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.e == 0x12) {
        printf("CPU opcodes test LD E,A passed\n");
    }   else {
        printf("CPU opcodes test LD E,A FAIL\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.h == 0x12) {
        printf("CPU opcodes test LD H,A passed\n");
    } else {
        printf("CPU opcodes test LD H FAIL\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.l == 0x12) {
        printf("CPU opcodes test LD L,A passed\n");
    } else {
        printf("CPU opcodes test LD L,A FAIL\n");
    }

    cpu.a = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x12) {
        printf("CPU opcodes test LD A,B passed\n");
    } else {
        printf("CPU opcodes test LD A,B FAIL\n");
    }

    cpu.a = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x12) {
        printf("CPU opcodes test LD A,C passed\n");
    } else {
        printf("CPU opcodes test LD A,C FAIL\n");
    }

    cpu.a = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x12) {
        printf("CPU opcodes test LD A,D passed\n");
    } else {
        printf("CPU opcodes test LD A,D FAIL\n");
    }

    cpu.a = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x12) {
        printf("CPU opcodes test LD A,E passed\n");
    } else {
        printf("CPU opcodes test LD A,E FAIL\n");
    }

    cpu.a = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x12) {
        printf("CPU opcodes test LD A,H passed\n");
    } else {
        printf("CPU opcodes test LD A,H FAIL\n");
    }

    cpu.a = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x12) {
        printf("CPU opcodes test LD A,L passed\n");
    } else {
        printf("CPU opcodes test LD A,L FAIL\n");
    }
}

void test_cpu_opcodes_3(void) {
    BUS BUS = {0};
    CPU cpu = {0};
    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x02;


    bus_write_byte(&BUS, cpu.pc, 0x7E);             //LD A, (HL) // Loads byte at address HL into A
    bus_write_byte(&BUS, cpu.pc + 1, 0x77); //LD (HL), A // Writes A into byte at address stored in HL
    bus_write_byte(&BUS, cpu.pc + 2, 0x42);

    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42) {
        printf("CPU opcodes test LD A, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD A, (HL) FAIL\n");
    }

    cpu.a = 0x43;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x43) {
        printf("CPU opcodes test LD (HL), A passed\n");
    } else {
        printf("CPU opcodes test LD (HL), A FAIL\n");
    }


    cpu.pc = 0xC005;
    cpu.h = 0xC0;
    cpu.l = 0x07;

    bus_write_byte(&BUS, 0xC005, 0x36); //LD (HL), d8 // Writes 8bit immediate value at address stored in HL
    bus_write_byte(&BUS, cpu.pc + 1, 0x45);

    cpu_step(&cpu, &BUS);
    uint8_t result = bus_read_byte(&BUS, cpu_get_HL(&cpu));
    printf("result: %02X\n", result);
    printf("%02X\n", bus_read_byte(&BUS, 0xC000 + 7));
    if (result == 0x45) {
        printf("CPU opcodes test LD (HL), d8 passed\n");
    } else {
        printf("CPU opcodes test LD (HL), D8 FAIL\n");
    }


}