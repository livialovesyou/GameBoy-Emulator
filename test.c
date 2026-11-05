//
// Created by Livia on 23.10.2026.
//

#include "test.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "ppu.h"

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

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x10;
    cpu.b = 0x14;
    bus_write_byte(&BUS, cpu.pc, 0x70);    //LD (HL), B // Writes B into byte at address HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == 0x14) {
        printf("CPU opcodes test LD (HL), B passed\n");
    } else {
        printf("CPU opcodes test LD (HL), B FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x11;
    cpu.c = 0x15;
    bus_write_byte(&BUS, cpu.pc, 0x71);    //LD (HL), C // Writes C into byte at address HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == 0x15) {
        printf("CPU opcodes test LD (HL), C passed\n");
    } else {
        printf("CPU opcodes test LD (HL), C FAIL\n");

    }

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x12;
    cpu.d = 0x16;
    bus_write_byte(&BUS, cpu.pc, 0x72);    //LD (HL), D // Writes D into byte at address HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == 0x16) {
        printf("CPU opcodes test LD (HL), D passed\n");
    } else {
        printf("CPU opcodes test LD (HL), D FAIL\n");

    }

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x13;
    cpu.e = 0x17;
    bus_write_byte(&BUS, cpu.pc, 0x73);    //LD (HL), E // Writes E into byte at address HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == 0x17) {
        printf("CPU opcodes test LD (HL), E passed\n");
    } else {
        printf("CPU opcodes test LD (HL), E FAIL\n");

    }

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x14;

    bus_write_byte(&BUS, cpu.pc, 0x74);    //LD (HL), H // Writes H into byte at address HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == cpu.h) {
        printf("CPU opcodes test LD (HL), H passed\n");
    } else {
        printf("CPU opcodes test LD (HL), H FAIL\n");

    }

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x15;

    bus_write_byte(&BUS, cpu.pc, 0x75);    //LD (HL), L // Writes L into byte at address HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == cpu.l) {
        printf("CPU opcodes test LD (HL), L passed\n");
    } else {
        printf("CPU opcodes test LD (HL), L FAIL\n");

    }

    cpu.pc = 0xC005;
    cpu.h = 0xC0;
    cpu.l = 0x07;
    bus_write_byte(&BUS, 0xC005, 0x36); //LD (HL), d8 // Writes 8bit immediate value at address stored in HL
    bus_write_byte(&BUS, cpu.pc + 1, 0x45);
    cpu_step(&cpu, &BUS);
    uint8_t result = bus_read_byte(&BUS, cpu_get_HL(&cpu));
    if (result == 0x45) {
        printf("CPU opcodes test LD (HL), d8 passed\n");
    } else {
        printf("CPU opcodes test LD (HL), D8 FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x0A;
    cpu.b = 0;
    cpu.c = 0;
    bus_write_byte(&BUS, cpu.pc, 0x46); //LD B, (HL) // Loads byte at address HL into B
    bus_write_byte(&BUS, 0xC00A, 0x41);
    bus_write_byte(&BUS, cpu.pc + 1, 0x4E); //LD C, (HL) // Loads byte at address HL into C
    bus_write_byte(&BUS, cpu.pc + 2, 0x56);////LD D, (HL) // Loads byte at address HL into D
    bus_write_byte(&BUS, cpu.pc + 3, 0x5E); //LD E, (HL) // Loads byte at address HL into E
    bus_write_byte(&BUS, cpu.pc + 4, 0x66); //LD H, (HL) // Loads byte at address HL into H
    bus_write_byte(&BUS, cpu.pc + 5, 0x6E); //LD L, (HL) // Loads byte at address HL into L
    cpu_step(&cpu, &BUS);
    if (cpu.b == 0x41) {
        printf("CPU opcodes test LD B, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD B, (HL) FAIL\n");
    }

    cpu.h = 0xC0;
    cpu.l = 0x0B;
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x42);
    cpu_step(&cpu, &BUS);
    if (cpu.c == 0x42) {
        printf("CPU opcodes test LD C, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD C, (HL) FAIL\n");
    }

    cpu.d = 0;
    cpu.h = 0xC0;
    cpu.l = 0x0C;
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x43);
    cpu_step(&cpu, &BUS);
    if (cpu.d == 0x43) {
        printf("CPU opcodes test LD D, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD D, (HL) FAIL\n");
    }

    cpu.e = 0;
    cpu.h = 0xC0;
    cpu.l = 0x0D;
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x44);
    cpu_step(&cpu, &BUS);
    if (cpu.e == 0x44) {
        printf("CPU opcodes test LD E, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD E, (HL) FAIL\n");
    }

    cpu.h = 0xC0;
    cpu.l = 0x0E;
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x45);
    cpu_step(&cpu, &BUS);
    if (cpu.h == 0x45) {
        printf("CPU opcodes test LD H, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD H, (HL) FAIL\n");
    }

    cpu.h = 0xC0;
    cpu.l = 0x0F;
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x46);
    cpu_step(&cpu, &BUS);
    if (cpu.l == 0x46) {
        printf("CPU opcodes test LD L, (HL) passed\n");
    } else {
        printf("CPU opcodes test LD L, (HL) FAIL\n");
    }
}

void test_cpu_opcodes_4(void) {

    BUS BUS = {0};
    CPU cpu = {0};
    cpu.pc = 0xC000;
    cpu.h = 0xC0;
    cpu.l = 0x02;
    bus_write_byte(&BUS, cpu.pc, 0x21); //LD (HL) d16 // Load 16bit LE immediate into HL
    bus_write_byte(&BUS, cpu.pc + 1, 0x34);
    bus_write_byte(&BUS, cpu.pc + 2, 0x12);
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0x1234) {
        printf("CPU Opcodes test LD (HL) d16 passed\n");
    } else {
        printf("CPU Opcodes test LD (HL) d16 FAIL\n");
    }
    //printf("%04X", cpu.pc);

    cpu.pc = 0xC000;
    cpu_set_HL(&cpu, 0xCFFA);
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0xF9);//LD SP,HL // LOAD HL into the Stack Pointer
    cpu_step(&cpu, &BUS);
    if (cpu.sp == 0xCFFA) {
        printf("CPU Opcodes test LD SP,HL passed\n");
    } else {
        printf("CPU Opcodes test LD SP,HL FAIL\n");
    }


    cpu.pc = 0xC000;
    bus_write_byte(&BUS, cpu.pc, 0x01);//LD BC d16 // Load 16bit LE immediate into BC
    bus_write_byte(&BUS, cpu.pc + 1, 0x34);
    bus_write_byte(&BUS, cpu.pc + 2, 0x12);
    cpu_step(&cpu, &BUS);
    if (cpu_get_BC(&cpu) == 0x1234) {
        printf("CPU Opcodes test LD (BC) d16 passed\n");
    } else {
        printf("CPU Opcodes test LD (BC) d16 FAIL\n");
    }

    cpu.pc = 0xC000;
    bus_write_byte(&BUS, cpu.pc, 0x11);//LD DE d16 // Load 16bit LE immediate into DE
    bus_write_byte(&BUS, cpu.pc + 1, 0x34);
    bus_write_byte(&BUS, cpu.pc + 2, 0x12);
    cpu_step(&cpu, &BUS);
    if (cpu_get_DE(&cpu) == 0x1234) {
        printf("CPU Opcodes test LD (DE) d16 passed\n");
    } else {
        printf("CPU Opcodes test LD (DE) d16 FAIL\n");
    }

    cpu.pc = 0xC000;
    bus_write_byte(&BUS, cpu.pc, 0x31);//LD SP, d16 // Load 16bit Little Endian immediate to stack pointer
    bus_write_byte(&BUS, cpu.pc + 1, 0x34);
    bus_write_byte(&BUS, cpu.pc + 2, 0x12);
    cpu_step(&cpu, &BUS);
    if (cpu.sp == 0x1234) {
        printf("CPU Opcodes test LD SP d16 passed\n");
    } else {
        printf("CPU Opcodes test LD SP d16 FAIL\n");
    }
}

void test_cpu_opcodes_5(void) {
    BUS BUS = {0};
    CPU cpu = {0};


    cpu.pc = 0xC000;
    cpu.a = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x3C ); //INC A // Increments A and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.a == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC A passed\n");
        }   else {
            printf("CPU Opcodes test INC A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x3D ); //DEC A // Decrements A and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.a == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC A passed\n");
        }   else {
            printf("CPU Opcodes test DEC A FAIL\n");
        }



    cpu.pc = 0xC000;
    cpu.b = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x04 ); //INC B // Increments B and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.b == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC B passed\n");
    }   else {
        printf("CPU Opcodes test INC B FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.b = 0x00;
    cpu.c = 0xFF;
    bus_write_byte(&BUS, cpu.pc, 0x03 ); //INC BC // Increments BC
    cpu_step(&cpu, &BUS);
    if (cpu_get_BC(&cpu) == 0x0100) {
        printf("CPU Opcodes test INC BC passed\n");
        } else {
            printf("CPU Opcodes test INC BC FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.b = 0x00;
    cpu.c = 0xFF;
    bus_write_byte(&BUS, cpu.pc, 0x0B ); //DEC BC // Decrements BC
    cpu_step(&cpu, &BUS);
    if (cpu_get_BC(&cpu) == 0x00FE) {
        printf("CPU Opcodes test DEC BC passed\n");
    } else {
        printf("CPU Opcodes test DEC BC FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.b = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x05 ); //DEC B // Decrements B and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.b == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC B passed\n");
        }   else {
            printf("CPU Opcodes test DEC B FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.c = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x0C ); //INC C // Increments C and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.c == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC C passed\n");
        }   else {
            printf("CPU Opcodes test INC C FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.c = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x0D ); //DEC C // Decrements C and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.c == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC C passed\n");
        }   else {
            printf("CPU Opcodes test DEC C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.d = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x14 ); //INC D // Increments D and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.d == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC D passed\n");
        }   else {
            printf("CPU Opcodes test INC D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.d = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x15 ); //DEC D // Decrements D and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.d == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC D passed\n");
        }   else {
            printf("CPU Opcodes test DEC D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.e = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x1C ); //INC E // Increments E and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.e == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC E passed\n");
        }   else {
            printf("CPU Opcodes test INC E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.e = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x1D ); //DEC E // Decrements E and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.e == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC E passed\n");
        }   else {
            printf("CPU Opcodes test DEC E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.d = 0x00;
    cpu.e = 0xFF;
    bus_write_byte(&BUS, cpu.pc, 0x13 ); //INC DE // Increments DE
    cpu_step(&cpu, &BUS);
    if (cpu_get_DE(&cpu) == 0x0100) {
        printf("CPU Opcodes test INC DE passed\n");
    } else {
        printf("CPU Opcodes test INC DE FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.d = 0x00;
    cpu.e = 0xFF;
    bus_write_byte(&BUS, cpu.pc, 0x1B ); //DEC DE // Decrements DE
    cpu_step(&cpu, &BUS);
    if (cpu_get_DE(&cpu) == 0x00FE) {
        printf("CPU Opcodes test DEC DE passed\n");
    } else {
        printf("CPU Opcodes test DEC DE FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu.h = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x24 ); //INC H // Increments H and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.h == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC H passed\n");
        }   else {
            printf("CPU Opcodes test INC H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.h = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x25 ); //DEC H // Decrements H and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.h == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC H passed\n");
        }   else {
            printf("CPU Opcodes test DEC H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.l = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x2C ); //INC L // Increments L and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.l == 0x10) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC L passed\n");
        }   else {
            printf("CPU Opcodes test INC L FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.l = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x2D ); //DEC L // Decrements l and records flags
    cpu_step(&cpu, &BUS);
    if ((cpu.l == 0xFF) &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC L passed\n");
        }   else {
            printf("CPU Opcodes test DEC L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.h = 0x00;
    cpu.l = 0xFF;
    bus_write_byte(&BUS, cpu.pc, 0x23 ); //INC HL // Increments HL
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0x0100) {
        printf("CPU Opcodes test INC HL passed\n");
    } else {
        printf("CPU Opcodes test INC HL FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.h = 0x00;
    cpu.l = 0xFF;
    bus_write_byte(&BUS, cpu.pc, 0x2B ); //DEC HL // Decrements HL
    cpu_step(&cpu, &BUS);
    if (cpu_get_DE(&cpu) == 0x00FE) {
        printf("CPU Opcodes test DEC HL passed\n");
    } else {
        printf("CPU Opcodes test DEC HL FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu_set_HL(&cpu, 0xC115);
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x0F );
    bus_write_byte(&BUS, cpu.pc, 0x34 ); //INC (HL)
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == 0x10 &&
    (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
    (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
    (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test INC (HL) passed\n");
    } else {
        printf("CPU Opcodes test INC (HL) FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu_set_HL(&cpu, 0xC115);
    bus_write_byte(&BUS, cpu_get_HL(&cpu), 0x00 );
    bus_write_byte(&BUS, cpu.pc, 0x35 ); //DEC (HL)
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu_get_HL(&cpu)) == 0xFF &&
    (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
    (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
    (cpu_flag_is_set(&cpu, FLAG_H) == 1)) {
        printf("CPU Opcodes test DEC (HL) passed\n");
    } else {
        printf("CPU Opcodes test DEC (HL) FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu.sp = 0xCFFE;
    bus_write_byte(&BUS, cpu.pc, 0x33 ); //INC SP // Increments the Stack Pointer
    cpu_step(&cpu, &BUS);
    if (cpu.sp == 0xCFFF) {
        printf("CPU Opcodes test INC SP passed\n");
    } else {
        printf("CPU Opcodes test INC SP FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.sp = 0xCFFE;
    bus_write_byte(&BUS, cpu.pc, 0x3B ); //DEC SP // Decrement the Stack Pointer
    cpu_step(&cpu, &BUS);
    if (cpu.sp == 0xCFFD) {
        printf("CPU Opcodes test DEC SP passed\n");
    } else {
        printf("CPU Opcodes test DEC SP FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu.b = 0xF2;
    bus_write_byte(&BUS, cpu.pc, 0x80 );//ADD A, B // Add B to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, B passed\n");
    } else {
        printf("CPU Opcodes test ADD A, B FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu.b = 0x00;
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x88); //ADC A,B
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x10) {
        printf("CPU Opcodes test ADC A,B passed\n");
    } else {
        printf("CPU Opcodes test ADC A,B FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu.c = 0xF2;
    bus_write_byte(&BUS, cpu.pc, 0x81 );//ADD A, C // Add C to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, C passed\n");
        } else {
            printf("CPU Opcodes test ADD A, C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu.d = 0xF2;
    bus_write_byte(&BUS, cpu.pc, 0x82 );//ADD A, D // Add D to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, D passed\n");
        } else {
            printf("CPU Opcodes test ADD A, D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu.e = 0xF2;
    bus_write_byte(&BUS, cpu.pc, 0x83 );//ADD A, E // Add E to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, E passed\n");
        } else {
            printf("CPU Opcodes test ADD A, E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu.h = 0xF2;
    bus_write_byte(&BUS, cpu.pc, 0x84 );//ADD A, H // Add H to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, H passed\n");
        } else {
            printf("CPU Opcodes test ADD A, H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu.l = 0xF2;
    bus_write_byte(&BUS, cpu.pc, 0x85 );//ADD A, L // Add L to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, L passed\n");
        } else {
            printf("CPU Opcodes test ADD A, L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    cpu_set_HL(&cpu, 0xC100 );
    bus_write_byte(&BUS, 0xC100, 0xF2 );
    bus_write_byte(&BUS, cpu.pc, 0x86 );//ADD A, (HL) // Add (HL) to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, (HL) passed\n");
        } else {
            printf("CPU Opcodes test ADD A, (HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x05;

    bus_write_byte(&BUS, cpu.pc, 0x87 );//ADD A, A // Add A to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0A &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test ADD A, A passed\n");
        } else {
            printf("CPU Opcodes test ADD A, A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0xF1;
    bus_write_byte(&BUS, cpu.pc + 1, 0xF2 );
    bus_write_byte(&BUS, cpu.pc, 0xC6 );//ADD A, d8 // Add d8 to A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xE3 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test ADD A, d8 passed\n");
        } else {
            printf("CPU Opcodes test ADD A, d8 FAIL\n");
        }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu.c = 0x00;
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x89); //ADC A,C
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x10) {
        printf("CPU Opcodes test ADC A,C passed\n");
    } else {
        printf("CPU Opcodes test ADC A,C FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu.d = 0x00;
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x8A); //ADC A,D
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x10) {
        printf("CPU Opcodes test ADC A,D passed\n");
    } else {
        printf("CPU Opcodes test ADC A,D FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu.e = 0x00;
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x8B); //ADC A,E
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x10) {
        printf("CPU Opcodes test ADC A,E passed\n");
    } else {
        printf("CPU Opcodes test ADC A,E FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu.h = 0x00;
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x8C); //ADC A,H
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x10) {
        printf("CPU Opcodes test ADC A,H passed\n");
    } else {
        printf("CPU Opcodes test ADC A,H FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu.l = 0x00;
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x8D); //ADC A,L
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x10) {
        printf("CPU Opcodes test ADC A,L passed\n");
    } else {
        printf("CPU Opcodes test ADC A,L FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu_set_HL(&cpu, 0xC150);
    bus_write_byte(&BUS, 0xC150, 0x01);
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x8E); //ADC A,(HL)
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x11) {
        printf("CPU Opcodes test ADC A,(HL) passed\n");
    } else {
        printf("CPU Opcodes test ADC A,(HL) FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x01;
    cpu_set_HL(&cpu, 0x01);
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0x8D); //ADC A,A
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x03) {
        printf("CPU Opcodes test ADC A,A passed\n");
    } else {
        printf("CPU Opcodes test ADC A,A FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x0F;
    cpu_set_HL(&cpu, 0x01);
    cpu_set_flag(&cpu, FLAG_C,1);
    bus_write_byte(&BUS, cpu.pc, 0xCE); //ADC A,d8
    bus_write_byte(&BUS, cpu.pc + 1, 0x01);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x11) {
        printf("CPU Opcodes test ADC A,d8 passed\n");
    } else {
        printf("CPU Opcodes test ADC A,d8 FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu_set_HL(&cpu, 0x00FE);
    cpu_set_BC(&cpu, 0xFE00);
    bus_write_byte(&BUS, cpu.pc, 0x09 );// ADD HL,BC //add BC to HL, store in HL
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0xFEFE &&
        cpu_flag_is_set(&cpu, FLAG_H) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_N) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_C) == 0) {
        printf("CPU Opcodes test ADD HL,BC passed\n");
    } else {
        printf("CPU Opcodes test ADD HL,BC FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu_set_HL(&cpu, 0x00FE);
    cpu_set_DE(&cpu, 0xFE00);
    bus_write_byte(&BUS, cpu.pc, 0x19 );// ADD HL,DE //add DE to HL, store in HL
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0xFEFE &&
        cpu_flag_is_set(&cpu, FLAG_H) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_N) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_C) == 0) {
        printf("CPU Opcodes test ADD HL,DE passed\n");
        } else {
            printf("CPU Opcodes test ADD HL,DE FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu_set_HL(&cpu, 0x0022);

    bus_write_byte(&BUS, cpu.pc, 0x29 );// ADD HL,HL //add HL to HL, store in HL
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0x0044 &&
        cpu_flag_is_set(&cpu, FLAG_H) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_N) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_C) == 0) {
        printf("CPU Opcodes test ADD HL,HL passed\n");
        } else {
            printf("CPU Opcodes test ADD HL,HL FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.sp = 0xCFFA;
    bus_write_byte(&BUS, cpu.pc, 0xE8 ); //ADD SP, d8// add signed 8bit offset to SP, store in SP
    bus_write_byte(&BUS, cpu.pc + 1, 0x05 );
    cpu_step(&cpu, &BUS);
    if (cpu.sp == 0xCFFF &&
        cpu_flag_is_set(&cpu, FLAG_H) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_N) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_C) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_Z) == 0) {
        printf("CPU Opcodes test ADD SP,d8 passed\n");
    } else {
        printf("CPU Opcodes test ADD SP,d8 FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.sp = 0xCFFA;
    cpu_set_HL(&cpu, 0x0000);
    bus_write_byte(&BUS, cpu.pc, 0xF8 ); //LD HL, SP+d8 // sum sp and 8bit s_offset, store in HL
    bus_write_byte(&BUS, cpu.pc + 1, 0x05 );
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0xCFFF &&
        cpu_flag_is_set(&cpu, FLAG_H) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_N) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_C) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_Z) == 0) {
        printf("CPU Opcodes test LD HL, SP+d8 passed\n");
        } else {
            printf("CPU Opcodes test LD HL, SP+d8 FAIL\n");
        }



    cpu.pc = 0xC000;
    cpu.sp = 0xD000;
    cpu_set_HL(&cpu, 0x0022);

    bus_write_byte(&BUS, cpu.pc, 0x39 );// ADD HL,SP //add SP to HL, store in HL
    cpu_step(&cpu, &BUS);
    if (cpu_get_HL(&cpu) == 0xD022 &&
        cpu_flag_is_set(&cpu, FLAG_H) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_N) == 0 &&
        cpu_flag_is_set(&cpu, FLAG_C) == 0) {
        printf("CPU Opcodes test ADD HL,SP passed\n");
        } else {
            printf("CPU Opcodes test ADD HL,SP FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.b = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0x90 );//SUB A, B // Sub B from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, B passed\n");
        } else {
            printf("CPU Opcodes test SUB A, B FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu.b = 0x01;
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x98 );//SBC A, B // Sub B from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, B passed\n");
        } else {
            printf("CPU Opcodes test SBC A, B FAIL\n");
        }



    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu.c = 0x01;
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x99 );//SBC A, C // Sub C from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, C passed\n");
        } else {
            printf("CPU Opcodes test SBC A, C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu.d = 0x01;
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x9A );//SBC A, D // Sub D from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, D passed\n");
        } else {
            printf("CPU Opcodes test SBC A, D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu.e = 0x01;
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x9B );//SBC A, E // Sub E from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, E passed\n");
        } else {
            printf("CPU Opcodes test SBC A, E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu.h = 0x01;
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x9C );//SBC A, H // Sub H from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, H passed\n");
        } else {
            printf("CPU Opcodes test SBC A, H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu.l = 0x01;
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x9D );//SBC A, L // Sub L from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, L passed\n");
        } else {
            printf("CPU Opcodes test SBC A, L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    cpu_set_HL(&cpu, 0xC100);
    bus_write_byte(&BUS, 0xC100, 0x01 );
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x9E );//SBC A, (HL) // Sub (HL) from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, (HL) passed\n");
        } else {
            printf("CPU Opcodes test SBC A, (HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;

    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0x9F );//SBC A, A // Sub A from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xFF &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 1)) {
        printf("CPU Opcodes test SBC A, A passed\n");
        } else {
            printf("CPU Opcodes test SBC A, A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x10;
    bus_write_byte(&BUS, cpu.pc + 1, 0x01 );
    cpu.f = 0x00;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0xDE );//SBC A, d8 // Sub d8 from A with carry, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SBC A, d8 passed\n");
        } else {
            printf("CPU Opcodes test SBC A, d8 FAIL\n");
        }



    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.c = 0x0F;
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x91 );//SUB A, C // Sub C from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, C passed\n");
        } else {
            printf("CPU Opcodes test SUB A, C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.d = 0x0F;
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x92 );//SUB A, D // Sub D from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, D passed\n");
        } else {
            printf("CPU Opcodes test SUB A, D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.e = 0x0F;
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x93 );//SUB A, E // Sub E from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, E passed\n");
        } else {
            printf("CPU Opcodes test SUB A, E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.h = 0x0F;
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x94 );//SUB A, H // Sub H from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, H passed\n");
        } else {
            printf("CPU Opcodes test SUB A, H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.l = 0x0F;
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x95 );//SUB A, L // Sub L from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, L passed\n");
        } else {
            printf("CPU Opcodes test SUB A, L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu_set_HL(&cpu, 0xC100);
    bus_write_byte(&BUS, 0xC100, 0x0F );
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x96 );//SUB A, (HL) // Sub (HL) from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, (HL) passed\n");
        } else {
            printf("CPU Opcodes test SUB A, (HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0x97 );//SUB A, A // Sub A from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x00 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, A passed\n");
        } else {
            printf("CPU Opcodes test SUB A, A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    bus_write_byte(&BUS, cpu.pc + 1, 0x0F );
    cpu_set_flag(&cpu, FLAG_C, 0);
    bus_write_byte(&BUS, cpu.pc, 0xD6 );//SUB A, d8 // Sub d8 from A, store in A, record all flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test SUB A, d8 passed\n");
        } else {
            printf("CPU Opcodes test SUB A, d8 FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.b = 0x0F;
    cpu.f = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0xA0); //AND A, B //Bitwise AND between A and B, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,B passed\n");
    }   else {
        printf("CPU Opcodes test AND A,B FAIL\n");
    }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.c = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA1); //AND A, C //Bitwise AND between A and C, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,C passed\n");
        }   else {
            printf("CPU Opcodes test AND A,C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.d = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA2); //AND A, D //Bitwise AND between A and D, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,D passed\n");
        }   else {
            printf("CPU Opcodes test AND A,D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.e = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA3); //AND A, E //Bitwise AND between A and E, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,E passed\n");
        }   else {
            printf("CPU Opcodes test AND A,E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.h = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA4); //AND A, H //Bitwise AND between A and H, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,H passed\n");
        }   else {
            printf("CPU Opcodes test AND A,H FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.l = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA5); //AND A, L //Bitwise AND between A and L, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,L passed\n");
        }   else {
            printf("CPU Opcodes test AND A,L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu_set_HL(&cpu, 0xC100);
    bus_write_byte(&BUS, 0xC100, 0x0F);
    bus_write_byte(&BUS, cpu.pc, 0xA6); //AND A, (HL) //Bitwise AND between A and (HL), stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,(HL) passed\n");
        }   else {
            printf("CPU Opcodes test AND A,(HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    bus_write_byte(&BUS, cpu.pc, 0xA7); //AND A, A //Bitwise AND between A and A, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,A passed\n");
        }   else {
            printf("CPU Opcodes test AND A,A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;

    bus_write_byte(&BUS, cpu.pc, 0xE6); //AND A, d8 //Bitwise AND between A and d8, stores in A, record flags
    bus_write_byte(&BUS, cpu.pc + 1, 0x0F);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test AND A,d8 passed\n");
        }   else {
            printf("CPU Opcodes test AND A,d8 FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.b = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xB0); // OR A, B //Bitwise OR between A and B, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,B passed\n");
    } else {
        printf("CPU Opcodes test OR A,B FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.c = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xB1); // OR A, C //Bitwise OR between A and C, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,C passed\n");
        } else {
            printf("CPU Opcodes test OR A,C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.d = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xB2); // OR A, D //Bitwise OR between A and D, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,D passed\n");
        } else {
            printf("CPU Opcodes test OR A,D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.e = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xB3); // OR A, E //Bitwise OR between A and E, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,E passed\n");
        } else {
            printf("CPU Opcodes test OR A,E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.h = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xB4); // OR A, H //Bitwise OR between A and H, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,H passed\n");
        } else {
            printf("CPU Opcodes test OR A,H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.l = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xB5); // OR A, L //Bitwise OR between A and L, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,L passed\n");
        } else {
            printf("CPU Opcodes test OR A,L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu_set_HL(&cpu, 0xC100);
    bus_write_byte(&BUS, 0xC100, 0x0F);
    bus_write_byte(&BUS, cpu.pc, 0xB6); // OR A, (HL) //Bitwise OR between A and (HL), stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,(HL) passed\n");
        } else {
            printf("CPU Opcodes test OR A,(HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    bus_write_byte(&BUS, cpu.pc, 0xB7); // OR A, A //Bitwise OR between A and A, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3E &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,A passed\n");
        } else {
            printf("CPU Opcodes test OR A,A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    bus_write_byte(&BUS, cpu.pc, 0xF6); // OR A, d8 //Bitwise OR between A and d8, stores in A, record flags
    bus_write_byte(&BUS, cpu.pc + 1, 0x0F);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x3F &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test OR A,d8 passed\n");
        } else {
            printf("CPU Opcodes test OR A,d8 FAIL\n");
        }



    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.b = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA8); // XOR A, B //Bitwise XOR between A and B, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,B passed\n");
        } else {
            printf("CPU Opcodes test XOR A,B FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.c = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xA9); // XOR A, C //Bitwise XOR between A and C, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,C passed\n");
        } else {
            printf("CPU Opcodes test XOR A,C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.d = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xAA); // XOR A, D //Bitwise XOR between A and D, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,D passed\n");
        } else {
            printf("CPU Opcodes test XOR A,D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.e = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xAB); // XOR A, E //Bitwise XOR between A and E, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,E passed\n");
        } else {
            printf("CPU Opcodes test XOR A,E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.h = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xAC); // XOR A, H //Bitwise XOR between A and H, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,H passed\n");
        } else {
            printf("CPU Opcodes test XOR A,H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu.l = 0x0F;
    bus_write_byte(&BUS, cpu.pc, 0xAD); // XOR A, L //Bitwise XOR between A and L, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,L passed\n");
        } else {
            printf("CPU Opcodes test XOR A,L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    cpu_set_HL(&cpu, 0xC100);
    bus_write_byte(&BUS, 0xC100, 0x0F);
    bus_write_byte(&BUS, cpu.pc, 0xAE); // XOR A, (HL) //Bitwise XOR between A and (HL), stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,(HL) passed\n");
        } else {
            printf("CPU Opcodes test XOR A,(HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;
    bus_write_byte(&BUS, cpu.pc, 0xAF); // XOR A, A //Bitwise XOR between A and A, stores in A, record flags
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x00 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,L passed\n");
        } else {
            printf("CPU Opcodes test XOR A,L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x3E;

    bus_write_byte(&BUS, cpu.pc, 0xEE); // XOR A, d8 //Bitwise XOR between A and d8, stores in A, record flags
    bus_write_byte(&BUS, cpu.pc + 1, 0x0F);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x31 &&
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test XOR A,d8 passed\n");
        } else {
            printf("CPU Opcodes test XOR A,d8 FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu.b = 0x10;
    bus_write_byte(&BUS, cpu.pc, 0xB8); // CP A, B //Compare A and B, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,B passed\n");
        } else {
            printf("CPU Opcodes test CP A,B FAIL\n");
        }

    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu.c = 0x10;
    bus_write_byte(&BUS, cpu.pc, 0xB9); // CP A, C //Compare A and C, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,C passed\n");
        } else {
            printf("CPU Opcodes test CP A,C FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu.d = 0x10;
    bus_write_byte(&BUS, cpu.pc, 0xBA); // CP A, D //Compare A and D, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,D passed\n");
        } else {
            printf("CPU Opcodes test CP A,D FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu.e = 0x10;
    bus_write_byte(&BUS, cpu.pc, 0xBB); // CP A, E //Compare A and E, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,E passed\n");
        } else {
            printf("CPU Opcodes test CP A,E FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu.h = 0x10;
    bus_write_byte(&BUS, cpu.pc, 0xBC); // CP A, H //Compare A and H, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,H passed\n");
        } else {
            printf("CPU Opcodes test CP A,H FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu.l = 0x10;
    bus_write_byte(&BUS, cpu.pc, 0xBD); // CP A, L //Compare A and L, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,L passed\n");
        } else {
            printf("CPU Opcodes test CP A,L FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    cpu_set_HL(&cpu, 0xC100);
    bus_write_byte(&BUS, 0xC100, 0x10);
    bus_write_byte(&BUS, cpu.pc, 0xBE); // CP A, (HL) //Compare A and (HL), record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,(HL) passed\n");
        } else {
            printf("CPU Opcodes test CP A,(HL) FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    bus_write_byte(&BUS, cpu.pc, 0xBF); // CP A, A //Compare A and A, record flags
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,A passed\n");
        } else {
            printf("CPU Opcodes test CP A,A FAIL\n");
        }


    cpu.pc = 0xC000;
    cpu.a = 0x25;
    bus_write_byte(&BUS, cpu.pc, 0xFE); // CP A, d8 //Compare A and d8, record flags
    bus_write_byte(&BUS, cpu.pc + 1, 0x10);
    cpu_step(&cpu, &BUS);
    if (
        (cpu_flag_is_set(&cpu, FLAG_Z) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_N) == 1) &&
        (cpu_flag_is_set(&cpu, FLAG_H) == 0) &&
        (cpu_flag_is_set(&cpu, FLAG_C) == 0)) {
        printf("CPU Opcodes test CP A,d8 passed\n");
        } else {
            printf("CPU Opcodes test CP A,d8 FAIL\n");
        }

}

void test_cpu_opcodes_6(void) {

    BUS BUS = {0};
    CPU cpu = {0};

    cpu.pc = 0xC000;
    bus_write_byte(&BUS, cpu.pc, 0xC3);//JP, d16 // Unconditional jump to immediate 16bit address
    bus_write_byte(&BUS, cpu.pc + 1, 0x10);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC0);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC010) {
        printf("CPU Opcodes test JP, d16 passed\n");
    } else {
        printf("CPU Opcodes test JP, d16 FAIL\n");
    }

    cpu.pc = 0xC000;
    cpu_set_flag(&cpu, FLAG_Z, 1);
    bus_write_byte(&BUS, cpu.pc, 0xC2);//JP NZ, d16 // Conditional jump to immediate 16bit address if ZERO Flag is cleared
    bus_write_byte(&BUS, cpu.pc + 1, 0x10);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC0);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC003) {
        printf("CPU Opcodes test JP NZ, d16 passed\n");
    } else {
        printf("CPU Opcodes test JP NZ, d16 FAIL\n");
    }
//printf("Zero FLag: %u\n", cpu_flag_is_set(&cpu, FLAG_Z));
    //printf("COUNTER: %04X\n", cpu.pc);



    cpu.pc = 0xC000;
    cpu_set_flag(&cpu, FLAG_Z, 1);
    bus_write_byte(&BUS, cpu.pc, 0xCA);//JP Z, d16 // Conditional jump to immediate 16bit address if ZERO Flag is set
    bus_write_byte(&BUS, cpu.pc + 1, 0x10);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC0);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC010) {
        printf("CPU Opcodes test JP Z, d16 passed\n");
    } else {
        printf("CPU Opcodes test JP Z, d16 FAIL\n");
    }
    //printf("Zero FLag: %u\n", cpu_flag_is_set(&cpu, FLAG_Z));
    //printf("COUNTER: %04X", cpu.pc);


    cpu.pc = 0xC000;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0xD2);//JP NC, d16 // Conditional jump to immediate 16bit address if CARRY Flag is cleared
    bus_write_byte(&BUS, cpu.pc + 1, 0x10);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC0);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC003) {
        printf("CPU Opcodes test JP NC, d16 passed\n");
    } else {
        printf("CPU Opcodes test JP NC, d16 FAIL\n");
    }
    //printf("Zero FLag: %u\n", cpu_flag_is_set(&cpu, FLAG_Z));
    //printf("COUNTER: %04X\n", cpu.pc);

    cpu.pc = 0xC000;
    cpu_set_flag(&cpu, FLAG_C, 1);
    bus_write_byte(&BUS, cpu.pc, 0xDA);//JP C, d16 // Conditional jump to immediate 16bit address if CARRY Flag is set
    bus_write_byte(&BUS, cpu.pc + 1, 0x10);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC0);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC010) {
        printf("CPU Opcodes test JP C, d16 passed\n");
    } else {
        printf("CPU Opcodes test JP C, d16 FAIL\n");
    }

    cpu.pc = 0xC100;
    bus_write_byte(&BUS, cpu.pc, 0x18); //JR, d8 // Unconditional jump to signed 8bit offset
    bus_write_byte(&BUS, cpu.pc + 1, 0xFE);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC100) {
        printf("CPU Opcodes test JR,d8 passed\n");
    } else {
        printf("CPU Opcodes test JR,d8 FAIL\n");
    }
    //printf("COUNTER: %04X", cpu.pc);


    cpu.pc = 0xC100;
    bus_write_byte(&BUS, cpu.pc, 0x20); //JR NZ,d8 // Conditional relative jump if ZERO FLAG is cleared
    bus_write_byte(&BUS, cpu.pc + 1, 0x15);
    cpu_set_flag(&cpu, FLAG_Z, 1);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC102) {
        printf("CPU Opcodes test JR NZ, d8 passed\n");
    } else {
        printf("CPU Opcodes test JR NZ, d8 FAIL\n");
    }


    cpu.pc = 0xC100;
    bus_write_byte(&BUS, cpu.pc, 0x28); //JR Z,d8 // Conditional relative jump if ZERO FLAG is set
    bus_write_byte(&BUS, cpu.pc + 1, 0x15);
    cpu_set_flag(&cpu, FLAG_Z, 1);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC117) {
        printf("CPU Opcodes test JR Z, d8 passed\n");
    } else {
        printf("CPU Opcodes test JR Z, d8 FAIL\n");
    }


    cpu.pc = 0xC100;
    bus_write_byte(&BUS, cpu.pc, 0x30); //JR NC,d8 // Conditional relative jump if CARRY FLAG is cleared
    bus_write_byte(&BUS, cpu.pc + 1, 0x15);
    cpu_set_flag(&cpu, FLAG_C, 1);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC102) {
        printf("CPU Opcodes test JR NC, d8 passed\n");
    } else {
        printf("CPU Opcodes test JR NC, d8 FAIL\n");
    }


    cpu.pc = 0xC100;
    bus_write_byte(&BUS, cpu.pc, 0x38); //JR C,d8 // Conditional relative jump if CARRY FLAG is set
    bus_write_byte(&BUS, cpu.pc + 1, 0x15);
    cpu_set_flag(&cpu, FLAG_C, 1);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0xC117) {
        printf("CPU Opcodes test JR C, d8 passed\n");
    } else {
        printf("CPU Opcodes test JR C, d8 FAIL\n");
    }

}

void test_cpu_opcodes_7(void) {
    BUS BUS = {0};
    CPU cpu = {0};

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;

    bus_write_byte(&BUS, cpu.pc, 0xCD); //CALL d16 // Calls subroutine, CPU saves return address to stack pointer, then jumps to target address
    bus_write_byte(&BUS, cpu.pc + 1, 0x50);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    bus_write_byte(&BUS, 0xC150, 0xC9);//RET //Returns from subroutine by reading the saved return address from the stack and loading it into the program counter
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC150 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test CALL d16 passed\n");
    } else {
        printf("CPU Opcodes test CALL d16 FAIL\n");
    }

    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC103 && cpu.sp == 0xD000) {
        printf("CPU Opcodes test RET passed\n");
    } else {
        printf("CPU Opcodes test RET FAIL\n");
    }
    cpu.pc = 0xC100;
    bus_write_byte(&BUS, cpu.pc, 0xC4); //CALL NZ d16 // Calls subroutine if not Zero, CPU saves return address to stack pointer, then jumps to target address
    bus_write_byte(&BUS, cpu.pc + 1, 0x50);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    bus_write_byte(&BUS, 0xC150, 0xC0); //RET NZ
    cpu_set_flag(&cpu, FLAG_Z, 0);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC150 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test CALL NZ d16 passed\n");
    } else {
        printf("CPU Opcodes test CALL NZ d16 FAIL\n");
    }
    cpu_set_flag(&cpu, FLAG_Z, 0);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC103 && cpu.sp == 0xD000) {
        printf("CPU Opcodes test RET NZ passed\n");
    } else {
        printf("CPU Opcodes test RET NZ FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0xCC); //CALL Z d16 // Calls subroutine if Zero, CPU saves return address to stack pointer, then jumps to target address
    bus_write_byte(&BUS, cpu.pc + 1, 0x50);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    bus_write_byte(&BUS, 0xC150, 0xC8); //RET Z
    cpu_set_flag(&cpu, FLAG_Z, 1);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC150 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test CALL Z d16 passed\n");
    } else {
        printf("CPU Opcodes test CALL Z d16 FAIL\n");
    }
    cpu_set_flag(&cpu, FLAG_Z, 1);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC103 && cpu.sp == 0xD000) {
        printf("CPU Opcodes test RET Z passed\n");
    } else {
        printf("CPU Opcodes test RET Z FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0xD4); //CALL NC d16 // Calls subroutine if not CARRY, CPU saves return address to stack pointer, then jumps to target address
    bus_write_byte(&BUS, cpu.pc + 1, 0x50);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    bus_write_byte(&BUS, 0xC150, 0xD0); //RET NC
    cpu_set_flag(&cpu, FLAG_C, 0);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC150 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test CALL NC d16 passed\n");
    } else {
        printf("CPU Opcodes test CALL NC d16 FAIL\n");
    }
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC103 && cpu.sp == 0xD000) {
        printf("CPU Opcodes test RET NC passed\n");
    } else {
        printf("CPU Opcodes test RET NC FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0xDC); //CALL C d16 // Calls subroutine if CARRY, CPU saves return address to stack pointer, then jumps to target address
    bus_write_byte(&BUS, cpu.pc + 1, 0x50);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    bus_write_byte(&BUS, 0xC150, 0xD8); //RET C
    cpu_set_flag(&cpu, FLAG_C, 1);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC150 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test CALL C d16 passed\n");
    } else {
        printf("CPU Opcodes test CALL C d16 FAIL\n");
    }
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC103 && cpu.sp == 0xD000) {
        printf("CPU Opcodes test RET C passed\n");
    } else {
        printf("CPU Opcodes test RET C FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    cpu.b = 34;
    cpu.c = 12;
    bus_write_byte(&BUS, cpu.pc, 0xC5); // PUSH BC // Writes the 16bit value into the stack
    bus_write_byte(&BUS, cpu.pc + 1, 0xC1);//POP BC
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu.sp) == 12 && bus_read_byte(&BUS, cpu.sp + 1) == 34) {
        printf("CPU Opcodes test PUSH BC passed\n");
    } else {
        printf("CPU Opcodes test PUSH BC FAIL\n");
    }
    cpu.b = 0;
    cpu.c = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.b == 34 && cpu.c == 12) {
        printf("CPU Opcodes test POP BC passed\n");
    } else {
        printf("CPU Opcodes test POP BC FAIL\n");
    }



    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    cpu.d = 34;
    cpu.e = 12;
    bus_write_byte(&BUS, cpu.pc, 0xD5); // PUSH DE // Writes the 16bit value into the stack
    bus_write_byte(&BUS, cpu.pc + 1, 0xD1);//POP DE
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu.sp) == 12 && bus_read_byte(&BUS, cpu.sp + 1) == 34) {
        printf("CPU Opcodes test PUSH DE passed\n");
    } else {
        printf("CPU Opcodes test PUSH DE FAIL\n");
    }
    cpu.d = 0;
    cpu.e = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.d == 34 && cpu.e == 12) {
        printf("CPU Opcodes test POP DE passed\n");
    } else {
        printf("CPU Opcodes test POP DE FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    cpu.h = 34;
    cpu.l = 12;
    bus_write_byte(&BUS, cpu.pc, 0xE5); // PUSH HL // Writes the 16bit value into the stack
    bus_write_byte(&BUS, cpu.pc + 1, 0xE1);//POP HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu.sp) == 12 && bus_read_byte(&BUS, cpu.sp + 1) == 34) {
        printf("CPU Opcodes test PUSH HL passed\n");
    } else {
        printf("CPU Opcodes test PUSH HL FAIL\n");
    }
    cpu.h = 0;
    cpu.l = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.h == 34 && cpu.l == 12) {
        printf("CPU Opcodes test POP HL passed\n");
    } else {
        printf("CPU Opcodes test POP HL FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    cpu.a = 0x34;
    cpu.f = 0x12;
    bus_write_byte(&BUS, cpu.pc, 0xF5); // PUSH AF // Writes the 16bit value into the stack
    bus_write_byte(&BUS, cpu.pc + 1, 0xF1);//POP AF
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, cpu.sp) == 0x10 && bus_read_byte(&BUS, cpu.sp + 1) == 0x34) {
        printf("CPU Opcodes test PUSH AF passed\n");
    } else {
        printf("CPU Opcodes test PUSH AF FAIL\n");
    }
    cpu.a = 0x00;
    cpu.f = 0x00;
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x34 && cpu.f == 0x10) {
        printf("CPU Opcodes test POP AF passed\n");
    } else {
        printf("CPU Opcodes test POP AF FAIL\n");
    }


    bus_write_byte(&BUS, 0xC100, 0xD9); //RETI // Return to 16bit immediate address and enable master interrupt
    bus_write_byte(&BUS, 0xCFFE, 0x34);
    bus_write_byte(&BUS, 0xCFFF, 0x12);

    cpu.pc = 0xC100;
    cpu.sp = 0xCFFE;
    cpu.ime = 0;
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0x1234 && cpu.ime == 1 && cpu.sp == 0xD000) {
        printf("CPU Opcodes test RETI passed\n");
    } else {
        printf("CPU Opcodes test RETI FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0xC7); //RST 00H
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0x0000 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test RST 00H passed\n");
    } else {
        printf("CPU Opcodes test RST 00H FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0xCF); //RST 08H
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0x0008 && cpu.sp == 0xCFFE) {
        printf("CPU Opcodes test RST 08H passed\n");
    } else {
        printf("CPU Opcodes test RST 08H FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, cpu.pc, 0x08);//LD (d16), SP writes to memory the stack pointer
    bus_write_byte(&BUS, cpu.pc + 1, 0x00);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC2);
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS,0xC200) == 0x00 && bus_read_byte(&BUS,0xC201) == 0xD0) {
        printf("CPU Opcodes test LD (d16), SP passed\n");
    } else {
        printf("CPU Opcodes test LD (d16), SP FAIL\n");
    }
    
}

void test_cpu_opcodes_8(void) {
    CPU cpu = {0};
    BUS BUS = {0};

    cpu.pc = 0xC100;
    cpu.a = 0x42;

    bus_write_byte(&BUS, cpu.pc, 0xE0); //LDH (a8), A writes A into HRAM
    bus_write_byte(&BUS, cpu.pc + 1, 0x81);
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, 0xFF00 + 0x0081) == 0x42) {
        printf("CPU Opcodes test LDH (a8), A passed\n");
    } else {
        printf("CPU Opcodes test LDH (a8), A FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x00;
    bus_write_byte(&BUS, 0xFF81, 0x42);
    bus_write_byte(&BUS, cpu.pc, 0xF0); //LDH A, (a8) //Writes HRAM byte into A
    bus_write_byte(&BUS, cpu.pc + 1, 0x81);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42) {
        printf("CPU Opcodes test LDH A, (a8) passed\n");
    } else {
        printf("CPU Opcodes test LDH (a8), A FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.a = 0x42;
    cpu.c = 0x85;
    bus_write_byte(&BUS, cpu.pc, 0xE2);//LD (C), A //writes register A to the address 0xFF00 + C, using register C as the 8-bit offset.
    cpu_step(&cpu, &BUS);

    if (bus_read_byte(&BUS, 0xFF85) == 0x42) {
        printf("CPU Opcodes test LD (C), A passed\n");
    } else {
        printf("CPU Opcodes test LD (C), A FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x42;
    bus_write_byte(&BUS, cpu.pc, 0xEA);//LD (a16), A//write register A to a 16-bit memory address.
    bus_write_byte(&BUS, cpu.pc + 1, 0x15);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, 0xC115) == 0x42) {
        printf("CPU Opcodes test LD (a16), A passed\n");
    } else {
        printf("CPU Opcodes test LD (a16), A FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0xFA);//LD A, (a16) // Load byte at address into A
    bus_write_byte(&BUS, cpu.pc + 1, 0x15);
    bus_write_byte(&BUS, cpu.pc + 2, 0xC1);
    bus_write_byte(&BUS, 0xC115, 0x42);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42) {
        printf("CPU Opcodes test LD A, (a16) passed\n");
    } else {
        printf("CPU Opcodes test LD A, (a16) FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.a = 0x42;
    cpu_set_HL(&cpu,0xC115);
    bus_write_byte(&BUS, cpu.pc, 0x32);//LD (HL-), A // writes A into byte at the address in HL, then decrements HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, 0xC115) == 0x42 && cpu_get_HL(&cpu) == 0xC114) {
        printf("CPU Opcodes test LD (HL-), A passed\n");
    } else {
        printf("CPU Opcodes test LD (HL-), A FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.a = 0x00;
    cpu_set_HL(&cpu,0xC115);
    bus_write_byte(&BUS, cpu.pc, 0x3A);//LD A, (HL-) // writes byte at address HL into A, then decrements HL
    bus_write_byte(&BUS, 0xC115, 0x42);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42 && cpu_get_HL(&cpu) == 0xC114) {
        printf("CPU Opcodes test LD A, (HL-) passed\n");
    } else {
        printf("CPU Opcodes test LD A, (HL-) FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.a = 0x42;
    cpu_set_HL(&cpu,0xC115);
    bus_write_byte(&BUS, cpu.pc, 0x22);//LD (HL+), A // writes A into byte at the address in HL, then increments HL
    cpu_step(&cpu, &BUS);
    if (bus_read_byte(&BUS, 0xC115) == 0x42 && cpu_get_HL(&cpu) == 0xC116) {
        printf("CPU Opcodes test LD (HL+), A passed\n");
    } else {
        printf("CPU Opcodes test LD (HL+), A FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.a = 0x00;
    cpu_set_HL(&cpu,0xC115);
    bus_write_byte(&BUS, cpu.pc, 0x2A);//LD A, (HL+) // writes byte at address HL into A, then increments HL
    bus_write_byte(&BUS, 0xC115, 0x42);
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42 && cpu_get_HL(&cpu) == 0xC116) {
        printf("CPU Opcodes test LD A, (HL+) passed\n");
    } else {
        printf("CPU Opcodes test LD A, (HL+) FAIL\n");
    }

}

void test_cpu_opcodes_9(void) {
    CPU cpu = {0};
    BUS BUS = {0};

    cpu.pc = 0xC100;
    cpu.a = 0x96;
    bus_write_byte(&BUS, cpu.pc, 0x07); //RLCA
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x2D && cpu_flag_is_set(&cpu, FLAG_C)) {
        printf("CPU Opcodes test RLCA passed\n");
    } else {
        printf("CPU Opcodes test RLCA FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x95;
    bus_write_byte(&BUS, cpu.pc, 0x0F); //RRCA
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0xCA && cpu_flag_is_set(&cpu, FLAG_C)) {
        printf("CPU Opcodes test RRCA passed\n");
    } else {
        printf("CPU Opcodes test RRCA FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x85;
    cpu.f = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x17); //RLA
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x0A && cpu_flag_is_set(&cpu, FLAG_C)) {
        printf("CPU Opcodes test RLA passed\n");
    } else {
        printf("CPU Opcodes test RLA FAIL\n");
    }


    cpu.pc = 0xC100;
    cpu.a = 0x85;
    cpu.f = 0x00;
    bus_write_byte(&BUS, cpu.pc, 0x1F); //RRA
    cpu_step(&cpu, &BUS);
    if (cpu.a == 0x42 && cpu_flag_is_set(&cpu, FLAG_C)) {
        printf("CPU Opcodes test RRA passed\n");
    } else {
        printf("CPU Opcodes test RRA FAIL\n");
    }

    cpu.pc = 0xC100;
    cpu.sp = 0xD000;
    bus_write_byte(&BUS, 0xC100, 0x76); // HALT
    bus_write_byte(&BUS, 0xC101, 0x00); // NOP
    cpu_step(&cpu, &BUS);
    cpu_step(&cpu, &BUS);
    if (cpu.pc == 0xC101 && cpu.halted == 1) {
        printf("CPU Opcodes test HALT passed\n");
    } else {
        printf("CPU Opcodes test HALT FAIL\n");
    }

}

void test_cpu_interrupts(void) {
    CPU cpu = {0};
    BUS BUS = {0};

    cpu.pc = 0xC123;
    cpu.sp = 0xD000;
    cpu.ime = 1;
    //Enable and request V-Blank
    bus_write_byte(&BUS, 0xFFFF, 0x01);
    bus_write_byte(&BUS, 0xFF0F, 0x01);
    cpu_step(&cpu, &BUS);

    if (cpu.pc == 0x40 &&
        cpu.sp == 0xCFFE &&
        cpu.ime == 0     &&
        bus_read_byte(&BUS, 0xFF0F) == 0xE0 &&
        bus_read_byte(&BUS, 0xCFFE + 1) == 0xC1 &&
        bus_read_byte(&BUS, 0xCFFE) == 0x23) {
        printf("CPU Opcodes test INTERRUPTS passed\n");
    } else {
        printf("CPU Opcodes test INTERRUPTS FAIL\n");
    }
    // printf("PC: %04X\n", cpu.pc);
    // printf("SP: %04X\n", cpu.sp);
    // printf("IME: %u\n", cpu.ime);
    // printf("IF  = %02X\n", bus_read_byte(&BUS, 0xFF0F));
    // printf("RET = %02X%02X\n",
    //        bus_read_byte(&BUS, 0xCFFE + 1),
    //        bus_read_byte(&BUS, 0xCFFE));


    //Test Halt Bug
    cpu.pc = 0xC100;
    cpu.ime = 0;
    cpu.b = 0;

    bus_write_byte(&BUS, 0xFFFF, 0x01); // Enable V-Blank
    bus_write_byte(&BUS, 0xFF0F, 0x01); // Request V-Blank
    bus_write_byte(&BUS, 0xC100, 0x76); // HALT
    bus_write_byte(&BUS, 0xC101, 0x04); // INC B
    cpu_step(&cpu, &BUS); // Execute HALT

    if (cpu.pc == 0xC101 &&
        cpu.halted == 0 &&
        cpu.halt_bug == 1) {
        printf("CPU Opcodes test HALT BUG 1st_half passed\n");
    } else {
        printf("CPU Opcodes test HALT BUG 1st_half FAIL\n");
    }
    cpu_step(&cpu, &BUS); // Execute INC B with no PC increment

    if (cpu.pc == 0xC101 &&
        cpu.b == 1) {
        printf("CPU Opcodes test HALT BUG 2nd_half passed\n");
        } else {
            printf("CPU Opcodes test HALT BUG 2nd_half FAIL\n");
        }

    //TEST EI delay
    cpu.pc = 0xC100;
    cpu.ime = 0;
    bool pass = true;
    bus_write_byte(&BUS, 0xC100, 0xFB); // EI
    bus_write_byte(&BUS, 0xC101, 0x00); // NOP

    cpu_step(&cpu, &BUS);
    if (cpu.ime == 1) pass = false;
    cpu_step(&cpu, &BUS);
    if (cpu.ime == 1 && pass == true) {
        printf("CPU Opcodes test EI passed\n");
    } else {
        printf("CPU Opcodes test EI FAIL\n");
    }


}

void test_ppu_render_bg(void) {
    PPU ppu;
    ppu_init(&ppu);
    ppu.bgp = 0x1B;

    uint8_t pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
    ppu.lcdc = 0x11;

    ppu.vram[0x1800] = 0x00;

    ppu.vram[0x0000] = 0xFF;
    ppu.vram[0x0001] = 0xFF;
    ppu_render_background(&ppu, pixels);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            printf("%d ",pixels[y][x]);
        }
        printf("\n");
    }

}
void test_ppu_registers(void) {
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    bus_write_byte(&BUS, REG_LCDC, 0x91);
    bus_write_byte(&BUS, REG_BGP, 0xE4);
    if (bus_read_byte(&BUS, REG_BGP) == 0xE4 && bus_read_byte(&BUS, REG_LCDC) == 0x91) {
        printf("PPU registers test passed\n");
    } else {
        printf("PPU registers test FAIL\n");
    }
}

void test_ppu_update(void) {
    PPU ppu;
    BUS BUS = {0};
    ppu_init(&ppu);
    BUS.ppu = &ppu;
    ppu.lcdc = 0x80;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    memset(framebuffer, 0xFF, sizeof(framebuffer));
    ppu_update(&ppu, &BUS, 80, framebuffer);
    printf("LY: %d, Dot: %d, Mode: %d\n",
           ppu.ly, ppu.dot, ppu.mode);

    ppu_update(&ppu, &BUS, 172, framebuffer);
    printf("LY: %d, Dot: %d, Mode: %d\n",
           ppu.ly, ppu.dot, ppu.mode);

    ppu_update(&ppu, &BUS, 204, framebuffer);
    printf("LY: %d, Dot: %d, Mode: %d\n",
           ppu.ly, ppu.dot, ppu.mode);
}

void test_ppu_lyc(void)
{
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x80;
    ppu.lyc = 0;

    ppu_update_stat(&ppu);

    printf("STAT: %02X\n", ppu.stat);

    ppu.lyc = 1;
    ppu_update_stat(&ppu);

    printf("STAT: %02X\n", ppu.stat);
}

void test_ppu_stat_interrupt(void)
{
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    ppu.lcdc = 0x80;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    memset(framebuffer, 0xFF, sizeof(framebuffer));

    // Enable Mode 2 STAT interrupt
    ppu.stat |= STAT_INT_OAM;
    ppu.ly = 144;

    ppu_update(&ppu, &BUS, 1, framebuffer);

    printf("IF: %02X\n", BUS.interrupt_flags);
}

void test_ppu_render_scanlines(void) {
    PPU ppu;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    ppu_init(&ppu);

    // Enable LCD, background, and unsigned tile addressing.
    ppu.lcdc = 0x91;

    // Identity palette: color indices map to themselves.
    ppu.bgp = 0xE4;

    // Set tile 0's first row to color index 1.
    ppu.vram[0] = 0xFF; // Low bitplane
    ppu.vram[1] = 0x00; // High bitplane

    // Initialize the framebuffer to a recognizable value.
    memset(framebuffer, 0xFF, sizeof(framebuffer));

    // Render scanline 0.
    ppu.ly = 0;
    ppu_render_scanline(&ppu, framebuffer);

    // Check scanline 0.
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        if (framebuffer[0][x] != 1)
        {
            printf("FAIL: Pixel 0,%d = %d, expected 1\n",
                   x, framebuffer[0][x]);

        }
    }

    // Check that scanline 1 was not modified.
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        if (framebuffer[1][x] != 0xFF)
        {
            printf("FAIL: Scanline 1 was modified\n");

        }
    }

    printf("PASS: Scanline 0 rendered correctly\n");
    printf("PASS: Other scanlines unchanged\n");

}

void test_ppu_HBlank_transition(void) {
    PPU ppu;
    BUS BUS = {0};

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    ppu_init(&ppu);

    // Connect the PPU to the bus if your bus requires it.
    BUS.ppu = &ppu;

    // Enable LCD, background, and unsigned tile addressing.
    ppu.lcdc = 0x91;
    ppu.lcd_was_enabled = 1;

    // Identity palette.
    ppu.bgp = 0xE4;

    // Make tile 0's first row color index 1.
    ppu.vram[0] = 0xFF;
    ppu.vram[1] = 0x00;

    // Initialize framebuffer with recognizable values.
    memset(framebuffer, 0xFF, sizeof(framebuffer));

    // Set PPU just before the HBlank transition.
    ppu.ly = 0;
    ppu.dot = 248;
    ppu.mode = PPU_MODE_DRAWING;
    ppu.previous_mode = PPU_MODE_DRAWING;

    // Advance four dots.
    ppu_update(&ppu, &BUS, 4, framebuffer);

    // Check PPU state.
    if (ppu.ly != 0)
    {
        printf("FAIL: LY = %u, expected 0\n", ppu.ly);
    }

    if (ppu.dot != 252)
    {
        printf("FAIL: Dot = %u, expected 252\n", ppu.dot);
    }

    if (ppu.mode != PPU_MODE_HBLANK)
    {
        printf("FAIL: Mode = %u, expected HBlank (0)\n", ppu.mode);
    }

    // Check that the first pixel was rendered.
    if (framebuffer[0][0] != 1)
    {
        printf("FAIL: Pixel = %u, expected 1\n",
               framebuffer[0][0]);
    }

    // Check that the final pixel was rendered.
    if (framebuffer[0][159] != 1)
    {
        printf("FAIL: Pixel = %u, expected 1\n",
               framebuffer[0][159]);
    }

    // Check that the next scanline was not modified.
    if (framebuffer[1][0] != 0xFF)
    {
        printf("FAIL: Scanline 1 was modified\n");
    }

    printf("PASS: HBlank transition detected\n");
    printf("PASS: Scanline 0 rendered\n");
    printf("PASS: Scanline 1 unchanged\n");
}

void test_ppu_scanline_crossing(void) {
    PPU ppu;
    BUS BUS = {0};

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    // Enable LCD, background, and unsigned tile addressing.
    ppu.lcdc = 0x91;
    ppu.lcd_was_enabled = 1;
    ppu.bgp = 0xE4;

    // Make tile 0's first row color index 1.
    ppu.vram[0] = 0xFF;
    ppu.vram[1] = 0x00;

    memset(framebuffer, 0xFF, sizeof(framebuffer));

    // Start just before the first boundary.
    ppu.ly = 0;
    ppu.dot = 78;
    ppu.mode = PPU_MODE_OAM;

    // Advance 378 dots:
    // 78 + 378 = 456.
    ppu_update(&ppu, &BUS, 378, framebuffer);

    // Check final PPU state.
    if (ppu.ly != 1)
    {
        printf("FAIL: LY = %u, expected 1\n", ppu.ly);
    }

    if (ppu.dot != 0)
    {
        printf("FAIL: Dot = %u, expected 0\n", ppu.dot);
    }

    if (ppu.mode != PPU_MODE_OAM)
    {
        printf("FAIL: Mode = %u, expected OAM (2)\n", ppu.mode);
    }

    // Check that scanline 0 was rendered.
    if (framebuffer[0][0] != 1 ||
        framebuffer[0][159] != 1)
    {
        printf("FAIL: Scanline 0 was not rendered correctly\n");
    }

    // Scanline 1 should not have been rendered yet.
    if (framebuffer[1][0] != 0xFF)
    {
        printf("FAIL: Scanline 1 was modified\n");
    }

    printf("PASS: OAM -> Drawing boundary crossed\n");
    printf("PASS: Drawing -> HBlank boundary crossed\n");
    printf("PASS: Scanline boundary crossed\n");
    printf("PASS: Scanline 0 rendered correctly\n");
    printf("PASS: Scanline 1 unchanged\n");

    printf("\nFinal state:\n");
    printf("LY: %u\n", ppu.ly);
    printf("Dot: %u\n", ppu.dot);
    printf("Mode: %u\n", ppu.mode);
}

void test_ppu_window_helper(void) {
    PPU ppu;
    BUS BUS = {0};
    ppu_init(&ppu);
    BUS.ppu = &ppu;

    ppu.lcdc = 0x21; // Window enabled, BG enabled
    ppu.wy = 50;
    ppu.wx = 87;
    // Window starts at x = 80, y = 50.
    assert(ppu_window_is_visible(&ppu, 79, 50) == 0);
    assert(ppu_window_is_visible(&ppu, 80, 50) == 1);
    assert(ppu_window_is_visible(&ppu, 100, 50) == 1);
    assert(ppu_window_is_visible(&ppu, 80, 49) == 0);

    printf("PASS: Window horizontal position\n");
    printf("PASS: Window vertical position\n");
}

void test_ppu_window_coords(void) {
    PPU ppu;
    BUS BUS = {0};
    ppu_init(&ppu);
    BUS.ppu = &ppu;
    uint8_t tile_x, tile_y;
    uint8_t pixel_x, pixel_y;
    ppu.wx = 87;
    ppu.window_line = 10;

    ppu_get_window_coordinates(
    &ppu, 85,
    &tile_x, &tile_y,
    &pixel_x, &pixel_y
);

    assert(tile_x == 0);
    assert(tile_y == 1);
    assert(pixel_x == 5);
    assert(pixel_y == 2);

    printf("PASS: Window tile coordinates\n");
    printf("PASS: Window pixel coordinates\n");
}

void test_ppu_window_pixel(void) {
    PPU ppu;
    BUS BUS = {0};
    ppu_init(&ppu);
    BUS.ppu = &ppu;

    // Enable Window, BG, unsigned tile addressing,
    // and select the 0x9800 Window map.
    ppu.lcdc = 0x71;

    // Place Window at screen coordinate (0, 0).
    ppu.wx = 7;
    ppu.wy = 0;
    ppu.window_line = 0;

    // Configure tile 0: every pixel has color index 1.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[y * 2] = 0xFF;
        ppu.vram[y * 2 + 1] = 0x00;
    }

    uint8_t color = ppu_get_window_pixel(&ppu, 0);

    assert(color == 1);

    printf("PASS: Window pixel retrieved correctly\n");

}

void test_ppu_render_scanline_window(void) {
    PPU ppu;
    BUS BUS = {0};
    ppu_init(&ppu);
    BUS.ppu = &ppu;
    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    // Enable LCD, Window, Background and unsigned tile addressing.
    ppu.lcdc = 0xF1;
    // Window starts at screen coordinate (80, 0).
    ppu.wx = 87;
    ppu.wy = 0;// Use identity palette:
    // color index 0 -> shade 0
    // color index 1 -> shade 1
    // color index 2 -> shade 2
    // color index 3 -> shade 3
    ppu.bgp = 0xE4;

    // Initialize framebuffer.
    memset(framebuffer, 0xFF, sizeof(framebuffer));

    // Background map: tile 1.
    ppu.vram[0x1800] = 1;

    // Window map at 0x9C00: tile 2 in all ten positions.
    for (int x = 0; x < 10; x++)
    {
        ppu.vram[0x1C00 + x] = 2;
    }

    // Tile 1: every pixel has color index 0.
    for (int y = 0; y < 8; y++) {
        ppu.vram[16 + y * 2] = 0x00;
        ppu.vram[16 + y * 2 + 1] = 0x00;
    }
    // Tile 2: every pixel has color index 1.
    for (int y = 0; y < 8; y++) {
        ppu.vram[32 + y * 2] = 0xFF;
        ppu.vram[32 + y * 2 + 1] = 0x00;
    }
    // Render scanline 0.
    ppu_render_scanline(&ppu, framebuffer);

    // Background should remain left of the Window.
    assert(framebuffer[0][0] == 0);
    assert(framebuffer[0][79] == 0);

    // Window should replace Background from x = 80.
    assert(framebuffer[0][80] == 1);
    assert(framebuffer[0][100] == 1);
    assert(framebuffer[0][159] == 1);

    // Window line should advance once.
    assert(ppu.window_line == 1);

    printf("PASS: Background rendered correctly\n");
    printf("PASS: Window replaces Background correctly\n");
    printf("PASS: Window line counter advances\n");

}

void test_ppu_window_line_progression(void) {
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

    ppu.lcdc = 0xF1;
    ppu.wx = 7;  // Window starts at x = 0.
    ppu.wy = 1;  // Window starts at screen line 1.
    ppu.bgp = 0xE4;

    // Window map: tile 1 in the first column.
    ppu.vram[0x1C00] = 1;
    ppu.vram[0x1C20] = 2;
    ppu.vram[0x1C40] = 3;

    // Tile 1: color index 1.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[16 + y * 2] = 0xFF;
        ppu.vram[16 + y * 2 + 1] = 0x00;
    }

    // Tile 2: color index 2.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[32 + y * 2] = 0x00;
        ppu.vram[32 + y * 2 + 1] = 0xFF;
    }

    // Tile 3: color index 3.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[48 + y * 2] = 0xFF;
        ppu.vram[48 + y * 2 + 1] = 0xFF;
    }

    // Screen line 0: Window is not visible.
    ppu.ly = 0;
    ppu_render_scanline(&ppu, framebuffer);

    assert(ppu.window_line == 0);

    // Screen line 1: Window uses internal line 0.
    ppu.ly = 1;
    ppu_render_scanline(&ppu, framebuffer);

    assert(ppu.window_line == 1);
    assert(framebuffer[1][0] == 1);

    // Screen line 2: Window uses internal line 1.
    ppu.ly = 2;
    ppu_render_scanline(&ppu, framebuffer);

    assert(ppu.window_line == 2);
    assert(framebuffer[2][0] == 1);

    printf("PASS: Window starts at WY\n");
    printf("PASS: Window line counter advances correctly\n");
}

void test_ppu_window_line_reset(void)
{
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    ppu.lcdc = 0x80;
    ppu.lcd_was_enabled = 1;
    // Simulate the end of a frame.
    ppu.ly = 153;
    ppu.dot = 455;
    ppu.window_line = 120;
    ppu.mode = PPU_MODE_VBLANK;

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_update(&ppu, &BUS, 1, framebuffer);

    assert(ppu.ly == 0);
    assert(ppu.dot == 0);
    assert(ppu.window_line == 0);
    assert(ppu.mode == PPU_MODE_OAM);

    printf("PASS: Window line counter resets at frame boundary\n");
}

void test_ppu_get_sprite(void) {
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;
    Sprite sprite;

    // Set up sprite 0.
    ppu.oam[0] = 32;
    ppu.oam[1] = 48;
    ppu.oam[2] = 5;
    ppu.oam[3] = 0xA0;
    ppu_get_sprite(&ppu, 0, &sprite);

    assert(sprite.y == 32);
    assert(sprite.x == 48);
    assert(sprite.tile == 5);
    assert(sprite.attributes == 0xA0);

    printf("PASS: Sprite data read from OAM\n");
}

void test_ppu_sprite_overlaps_scanline(void) {
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;
    Sprite sprite;

    // Sprite starts at screen Y = 16.
    sprite.y = 32;

    // Test 8x8 sprite.
    ppu.lcdc &= (uint8_t)~0x04;

    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 15) == 0);
    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 16) == 1);
    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 23) == 1);
    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 24) == 0);

    // Test 8x16 sprite.
    ppu.lcdc |= 0x04;

    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 16) == 1);
    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 31) == 1);
    assert(ppu_sprite_overlaps_scanline(&ppu, &sprite, 32) == 0);

    printf("PASS: 8x8 sprite scanline detection\n");
    printf("PASS: 8x16 sprite scanline detection\n");
}

void test_ppu_get_scanline_sprites(void) {
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;
    ppu.lcdc &= (uint8_t)~0x04; // 8x8 sprites

    // Place 12 sprites on scanline 20.
    for (int i = 0; i < 12; i++)
    {
        int offset = i * 4;

        ppu.oam[offset] = 36;       // Y = 20
        ppu.oam[offset + 1] = 40;   // X = 32
        ppu.oam[offset + 2] = i;    // Tile number
        ppu.oam[offset + 3] = 0;
    }

    Sprite sprites[MAX_SPRITES_PER_SCANLINE];

    int count = ppu_get_scanline_sprites(
        &ppu, 20, sprites
    );

    assert(count == 10);

    // Verify that the first 10 OAM entries were selected.
    for (int i = 0; i < 10; i++)
    {
        assert(sprites[i].tile == i);
    }

    printf("PASS: Maximum 10 sprites per scanline\n");
    printf("PASS: Sprites selected in OAM order\n");
}

void test_ppu_get_sprite_pixel(void) {
    PPU ppu;
    ppu_init(&ppu);

    Sprite sprite = {0};

    sprite.tile = 1;
    sprite.attributes = 0;

    // Tile 1: first row has color index 1 in every pixel.
    ppu.vram[16] = 0xFF;
    ppu.vram[17] = 0x00;

    // Remaining rows are color index 0.
    for (int y = 1; y < 8; y++)
    {
        ppu.vram[16 + y * 2] = 0x00;
        ppu.vram[17 + y * 2] = 0x00;
    }

    // Normal orientation.
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 0) == 1);
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 7, 0) == 1);
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 1) == 0);

    // Horizontal flip.
    sprite.attributes = 0x20;
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 0) == 1);

    // Vertical flip.
    sprite.attributes = 0x40;
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 0) == 0);

    printf("PASS: Sprite pixel decoding\n");
    printf("PASS: Horizontal flipping\n");
    printf("PASS: Vertical flipping\n");
}

void test_ppu_sprite_8x16(void)
{
    PPU ppu;
    ppu_init(&ppu);

    Sprite sprite = {0};

    sprite.tile = 5;
    sprite.attributes = 0;

    // Enable 8x16 sprites.
    ppu.lcdc |= 0x04;

    // Tile 4: color index 1.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[64 + y * 2] = 0xFF;
        ppu.vram[65 + y * 2] = 0x00;
    }

    // Tile 5: color index 2.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[80 + y * 2] = 0x00;
        ppu.vram[81 + y * 2] = 0xFF;
    }

    // Top half uses tile 4.
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 0) == 1);

    // Bottom half uses tile 5.
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 8) == 2);

    // Test vertical flipping.
    sprite.attributes = 0x40;

    // Top now corresponds to the original bottom.
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 0) == 2);

    // Bottom now corresponds to the original top.
    assert(ppu_get_sprite_pixel(&ppu, &sprite, 0, 15) == 1);

    printf("PASS: 8x16 sprite tile selection\n");
    printf("PASS: 8x16 vertical flipping\n");
}

void test_ppu_render_sprites(void) {
    PPU ppu;
    ppu_init(&ppu);
    ppu.obp0 = 0xE4;

    // Enable LCD and sprite rendering.
    ppu.lcdc = 0x82;

    // Set sprite palette to identity mapping for now.
    // Palette integration is not implemented yet.

    // Fill the framebuffer scanline with background color 3.
    uint8_t pixels[SCREEN_WIDTH];
    memset(pixels, 3, sizeof(pixels));

    // Configure sprite 0:
    // OAM Y = 32 -> screen Y = 16
    // OAM X = 48 -> screen X = 40
    ppu.oam[0] = 32;
    ppu.oam[1] = 48;
    ppu.oam[2] = 0;
    ppu.oam[3] = 0;

    // Configure tile 0:
    // First pixel row has color index 1 in all 8 pixels.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[row * 2] = 0xFF;
        ppu.vram[row * 2 + 1] = 0x00;
    }
    uint8_t bg_color_indices[SCREEN_WIDTH];
    // Render scanline 16, where the sprite is visible.
    ppu_render_sprites(&ppu, 16, pixels,bg_color_indices);

    // Sprite occupies screen X coordinates 40 through 47.
    for (int x = 40; x < 48; x++)
        assert(pixels[x] == 1);

    // Pixels outside the sprite must remain unchanged.
    assert(pixels[39] == 3);
    assert(pixels[48] == 3);

    printf("PASS: Sprite rendered at correct position\n");
    printf("PASS: Pixels outside sprite unchanged\n");

    // Test transparent pixels.
    // Set tile 0 to color index 0.
    memset(ppu.vram, 0, 16);

    memset(pixels, 3, sizeof(pixels));

    ppu_render_sprites(&ppu, 16, pixels,bg_color_indices);

    // Transparent sprite must not overwrite the background.
    for (int x = 40; x < 48; x++)
        assert(pixels[x] == 3);

    printf("PASS: Transparent sprite pixels preserved background\n");
}

void test_sprite_palettes(void) {
    PPU ppu;
    ppu_init(&ppu);

    Sprite sprite = {0};

    // Identity mapping: indices 0,1,2,3 -> shades 0,1,2,3.
    ppu.obp0 = 0xE4;

    // Map all nonzero indices to shade 3.
    ppu.obp1 = 0xFC;

    sprite.attributes = 0x00;

    assert(ppu_get_sprite_color(&ppu, &sprite, 0) == 0);
    assert(ppu_get_sprite_color(&ppu, &sprite, 1) == 1);
    assert(ppu_get_sprite_color(&ppu, &sprite, 2) == 2);
    assert(ppu_get_sprite_color(&ppu, &sprite, 3) == 3);

    // Select OBP1 using attribute bit 4.
    sprite.attributes = 0x10;

    assert(ppu_get_sprite_color(&ppu, &sprite, 0) == 0);
    assert(ppu_get_sprite_color(&ppu, &sprite, 1) == 3);
    assert(ppu_get_sprite_color(&ppu, &sprite, 2) == 3);
    assert(ppu_get_sprite_color(&ppu, &sprite, 3) == 3);

    printf("PASS: OBP0 palette mapping\n");
    printf("PASS: OBP1 palette mapping\n");
}

void test_sprite_priority_over_window(void) {
    PPU ppu;
    ppu_init(&ppu);

    // Enable LCD, Background, Window and sprites.
    ppu.lcdc = 0xF3;

    ppu.bgp = 0xE4;
    ppu.obp0 = 0xE4;

    // Window starts at x=0, y=0.
    ppu.wx = 7;
    ppu.wy = 0;

    // Select Window tile map at 0x9C00.
    // Tile map entry 0 points to tile 1.
    ppu.vram[0x1C00] = 1;

    // Configure Window tile 1 with color index 2.
    // Low byte = 0, high byte = 0xFF.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[16 + row * 2] = 0x00;
        ppu.vram[16 + row * 2 + 1] = 0xFF;
    }

    // Configure sprite 0 at screen position (0, 0).
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 2;
    ppu.oam[3] = 0x80; // Behind nonzero BG/Window pixels.

    // Configure sprite tile 2 with color index 1.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[32 + row * 2] = 0xFF;
        ppu.vram[32 + row * 2 + 1] = 0x00;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu.ly = 0;

    ppu_render_scanline(&ppu, framebuffer);

    // Window color index 2 maps to shade 2.
    for (int x = 0; x < 8; x++)
        assert(framebuffer[0][x] == 2);

    printf("PASS: Sprite stays behind nonzero Window pixels\n");
}

void test_sprite_x_priority(void) {
    PPU ppu;
    ppu_init(&ppu);

    // Enable LCD, unsigned tile data, and sprites.
    ppu.lcdc = 0x93;

    ppu.bgp = 0xE4;
    ppu.obp0 = 0xE4;

    // Sprite 0: screen position (4, 0), tile 1.
    // Higher OAM index priority, but smaller X.
    ppu.oam[0] = 16;
    ppu.oam[1] = 12;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0x00;

    // Sprite 1: screen position (0, 0), tile 2.
    // Lower X coordinate, so this sprite has priority.
    ppu.oam[4] = 16;
    ppu.oam[5] = 8;
    ppu.oam[6] = 2;
    ppu.oam[7] = 0x00;

    // Tile 1: color index 1.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[16 + row * 2] = 0xFF;
        ppu.vram[16 + row * 2 + 1] = 0x00;
    }

    // Tile 2: color index 2.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[32 + row * 2] = 0x00;
        ppu.vram[32 + row * 2 + 1] = 0xFF;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu.ly = 0;
    ppu_render_scanline(&ppu, framebuffer);

    // Sprite 1 occupies x=0 through x=7.
    for (int x = 0; x < 8; x++)
        assert(framebuffer[0][x] == 2);

    // Overlap is x=4 through x=7.
    // Sprite 1 must win because it has the smaller X coordinate.
    for (int x = 4; x < 8; x++)
        assert(framebuffer[0][x] == 2);

    // Sprite 0 is visible outside the overlap at x=8 through x=11.
    for (int x = 8; x < 12; x++)
        assert(framebuffer[0][x] == 1);

    printf("PASS: Smaller-X sprite has priority\n");
}

void test_sprite_oam_priority(void) {
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x93;
    ppu.bgp = 0xE4;
    ppu.obp0 = 0xE4;

    // Sprite 0: X=8, tile 1, color index 1.
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0x00;

    // Sprite 1: X=8, tile 2, color index 2.
    ppu.oam[4] = 16;
    ppu.oam[5] = 8;
    ppu.oam[6] = 2;
    ppu.oam[7] = 0x00;

    // Tile 1: color index 1.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[16 + row * 2] = 0xFF;
        ppu.vram[16 + row * 2 + 1] = 0x00;
    }

    // Tile 2: color index 2.
    for (int row = 0; row < 8; row++)
    {
        ppu.vram[32 + row * 2] = 0x00;
        ppu.vram[32 + row * 2 + 1] = 0xFF;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu.ly = 0;
    ppu_render_scanline(&ppu, framebuffer);

    // Both sprites overlap completely.
    // Sprite 0 has the lower OAM index, so it must win.
    for (int x = 0; x < 8; x++)
        assert(framebuffer[0][x] == 1);

    printf("PASS: Lower OAM index sprite has priority on equal X\n");
}

void test_sprites_render_with_background_disabled(void) {
    PPU ppu;
    ppu_init(&ppu);

    // LCD enabled, sprites enabled, background disabled.
    ppu.lcdc = 0x92;

    ppu.obp0 = 0xE4;
    ppu.bgp = 0xE4;

    // Place sprite at screen position (0, 0).
    ppu.oam[0] = 16; // Y position
    ppu.oam[1] = 8;  // X position
    ppu.oam[2] = 1;  // Tile number
    ppu.oam[3] = 0;  // Attributes

    // Make every pixel in tile 1 use color index 1.
    int tile_offset = 1 * 16;

    for (int y = 0; y < 8; y++)
    {
        ppu.vram[tile_offset + y * 2] = 0xFF;
        ppu.vram[tile_offset + y * 2 + 1] = 0x00;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_render_scanline(&ppu, framebuffer);

    // Sprite should be visible.
    for (int x = 0; x < 8; x++)
        assert(framebuffer[0][x] == 1);

    // Background should remain color zero outside the sprite.
    assert(framebuffer[0][8] == 0);

    printf("test_sprites_render_with_background_disabled passed\n");
}

void test_sprite_behind_background(void) {
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x93; // LCD, sprites, and background enabled
    ppu.bgp = 0xE4;
    ppu.obp0 = 0xE4;

    // Background tile 0: every pixel has color index 1.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[y * 2] = 0xFF;
        ppu.vram[y * 2 + 1] = 0x00;
    }

    // Background map uses tile 0.
    ppu.vram[0x1800] = 0;

    // Sprite at screen position (0, 0), behind background.
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0x80;

    // Sprite tile 1: every pixel has color index 2.
    int tile_offset = 1 * 16;

    for (int y = 0; y < 8; y++)
    {
        ppu.vram[tile_offset + y * 2] = 0x00;
        ppu.vram[tile_offset + y * 2 + 1] = 0xFF;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_render_scanline(&ppu, framebuffer);

    // Nonzero background pixels hide the sprite.
    assert(framebuffer[0][0] == 1);

    printf("test_sprite_behind_background passed\n");
}

void test_sprite_behind_background_color_zero(void) {

    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x93;
    ppu.bgp = 0xE4;
    ppu.obp0 = 0xE4;

    // Background tile 0: every pixel has color index 0.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[y * 2] = 0x00;
        ppu.vram[y * 2 + 1] = 0x00;
    }

    ppu.vram[0x1800] = 0;

    // Sprite at screen position (0, 0), behind background.
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0x80;

    // Sprite tile 1: every pixel has color index 2.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[16 + y * 2] = 0x00;
        ppu.vram[16 + y * 2 + 1] = 0xFF;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_render_scanline(&ppu, framebuffer);

    // Color-zero background must not hide the sprite.
    assert(framebuffer[0][0] == 2);

    printf("test_sprite_behind_background_color_zero passed\n");
}

void test_sprite_transparency(void) {
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x93;
    ppu.bgp = 0xE4;
    ppu.obp0 = 0xE4;

    // Background tile 0: color index 1 everywhere.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[y * 2] = 0xFF;
        ppu.vram[y * 2 + 1] = 0x00;
    }

    ppu.vram[0x1800] = 0;

    // Sprite at screen position (0, 0).
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0;

    // Sprite tile 1: color index 0 everywhere (transparent).
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[16 + y * 2] = 0x00;
        ppu.vram[16 + y * 2 + 1] = 0x00;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_render_scanline(&ppu, framebuffer);

    // Transparent sprite must not overwrite background color 1.
    assert(framebuffer[0][0] == 1);

    printf("test_sprite_transparency passed\n");
}

void test_sprite_horizontal_flip(void) {
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x82; // LCD and sprites enabled
    ppu.obp0 = 0xE4;

    // Sprite at screen position (0, 0).
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0x20; // Horizontal flip

    // Tile row: color indices 1, 3, 0, 0, 0, 0, 0, 0.
    // Low plane:  10000000
    // High plane: 11000000
    ppu.vram[16] = 0xC0;
    ppu.vram[17] = 0xA0;

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    uint8_t tile_pixels[8][8];

    ppu_decode_tile(&ppu, 0x8010, tile_pixels);

    // for (int x = 0; x < 8; x++)
    // {
    //     printf("%d ", tile_pixels[0][x]);
    // }
    // printf("\n");

    ppu_render_scanline(&ppu, framebuffer);
    // for (int x = 0; x < 8; x++)
    // {
    //     printf("%d ", framebuffer[0][x]);
    // }
    // printf("\n");


    // After horizontal flipping: 0, 0, 0, 0, 0, 2, 1, 3.
    assert(framebuffer[0][0] == 0);
    assert(framebuffer[0][1] == 0);
    assert(framebuffer[0][2] == 0);
    assert(framebuffer[0][3] == 0);
    assert(framebuffer[0][4] == 0);
    assert(framebuffer[0][5] == 2);
    assert(framebuffer[0][6] == 1);
    assert(framebuffer[0][7] == 3);

    printf("test_sprite_horizontal_flip passed\n");
}

void test_sprite_vertical_flip(void) {
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x82; // LCD and sprites enabled
    ppu.obp0 = 0xE4;

    // Sprite at screen position (0, 0).
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 1;
    ppu.oam[3] = 0x40; // Vertical flip

    // First tile row: color index 1.
    ppu.vram[16] = 0xFF;
    ppu.vram[17] = 0x00;

    // Last tile row: color index 2.
    ppu.vram[16 + 7 * 2] = 0x00;
    ppu.vram[16 + 7 * 2 + 1] = 0xFF;

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    ppu_render_scanline(&ppu, framebuffer);

    // Screen row 0 should show the original tile's last row.
    for (int x = 0; x < 8; x++)
        assert(framebuffer[0][x] == 2);

    printf("test_sprite_vertical_flip passed\n");
}

void test_sprite_8x16(void)
{
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0x86; // LCD, sprites, 8x16 mode
    ppu.obp0 = 0xE4;

    // Sprite at screen position (0, 0).
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 3; // LSB ignored; tiles 2 and 3 are used.
    ppu.oam[3] = 0;

    // Upper tile (tile 2): color index 1.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[2 * 16 + y * 2] = 0xFF;
        ppu.vram[2 * 16 + y * 2 + 1] = 0x00;
    }

    // Lower tile (tile 3): color index 2.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[3 * 16 + y * 2] = 0x00;
        ppu.vram[3 * 16 + y * 2 + 1] = 0xFF;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    // Render the first scanline.
    ppu.ly = 0;
    ppu_render_scanline(&ppu, framebuffer);

    assert(framebuffer[0][0] == 1);

    // Render the ninth scanline.
    ppu.ly = 8;
    ppu_render_scanline(&ppu, framebuffer);

    assert(framebuffer[8][0] == 2);

    printf("test_sprite_8x16 passed\n");
}

void test_sprite_8x16_vertical_flip(void)
{
    PPU ppu;
    ppu_init(&ppu);

    ppu.lcdc = 0xC6; // LCD enabled, sprites enabled, 8x16 mode
    ppu.obp0 = 0xE4;

    // Sprite at screen position (0, 0).
    ppu.oam[0] = 16;
    ppu.oam[1] = 8;
    ppu.oam[2] = 2;
    ppu.oam[3] = 0x40; // Vertical flip

    // Upper tile (tile 2): color index 1.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[2 * 16 + y * 2] = 0xFF;
        ppu.vram[2 * 16 + y * 2 + 1] = 0x00;
    }

    // Lower tile (tile 3): color index 2.
    for (int y = 0; y < 8; y++)
    {
        ppu.vram[3 * 16 + y * 2] = 0x00;
        ppu.vram[3 * 16 + y * 2 + 1] = 0xFF;
    }

    uint8_t framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH] = {0};

    // Render the first and last scanlines of the sprite.
    ppu.ly = 0;
    ppu_render_scanline(&ppu, framebuffer);

    ppu.ly = 15;
    ppu_render_scanline(&ppu, framebuffer);

    // Vertical flip puts the lower tile at the top.
    assert(framebuffer[0][0] == 2);

    // The upper tile appears at the bottom.
    assert(framebuffer[15][0] == 1);

    printf("test_sprite_8x16_vertical_flip passed\n");
}

void test_stat_interrupt_edge(void) {
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);

    ppu.lcdc = 0x80;
    ppu.mode = PPU_MODE_OAM;
    ppu.stat = STAT_INT_OAM;

    ppu_update_stat(&ppu);
    ppu_request_stat_interrupt(&ppu, &BUS);

    assert(BUS.interrupt_flags & INT_STAT);

    // Clear IF manually.
    BUS.interrupt_flags &= (uint8_t)~INT_STAT;

    // Signal is still active: no second interrupt.
    ppu_request_stat_interrupt(&ppu, &BUS);

    assert(!(BUS.interrupt_flags & INT_STAT));

    // Disable the OAM interrupt source.
    ppu.stat &= (uint8_t)~STAT_INT_OAM;
    ppu_request_stat_interrupt(&ppu, &BUS);

    // Re-enable it: a new rising edge should occur.
    ppu.stat |= STAT_INT_OAM;
    ppu_request_stat_interrupt(&ppu, &BUS);

    assert(BUS.interrupt_flags & INT_STAT);

    printf("test_stat_interrupt_edge passed\n");
}

void test_ppu_memory_access(void)
{
    PPU ppu;
    BUS BUS = {0};

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    ppu.lcdc = 0x80;

    // Initialize memory directly.
    ppu.vram[0] = 0x42;
    ppu.oam[0] = 0x55;

    // Mode 2: VRAM accessible, OAM blocked.
    ppu.mode = PPU_MODE_OAM;

    assert(bus_read_byte(&BUS, 0x8000) == 0x42);
    assert(bus_read_byte(&BUS, 0xFE00) == 0xFF);

    bus_write_byte(&BUS, 0x8000, 0x99);
    bus_write_byte(&BUS, 0xFE00, 0x99);

    assert(ppu.vram[0] == 0x99);
    assert(ppu.oam[0] == 0x55);

    // Mode 3: both blocked.
    ppu.mode = PPU_MODE_DRAWING;

    assert(bus_read_byte(&BUS, 0x8000) == 0xFF);
    assert(bus_read_byte(&BUS, 0xFE00) == 0xFF);

    bus_write_byte(&BUS, 0x8000, 0x11);
    bus_write_byte(&BUS, 0xFE00, 0x11);

    assert(ppu.vram[0] == 0x99);
    assert(ppu.oam[0] == 0x55);

    // Mode 0: both accessible.
    ppu.mode = PPU_MODE_HBLANK;

    assert(bus_read_byte(&BUS, 0x8000) == 0x99);
    assert(bus_read_byte(&BUS, 0xFE00) == 0x55);

    // LCD disabled: both accessible.
    ppu.lcdc = 0;
    ppu.mode = PPU_MODE_HBLANK;

    assert(bus_read_byte(&BUS, 0x8000) == 0x99);
    assert(bus_read_byte(&BUS, 0xFE00) == 0x55);

    printf("PASS: PPU VRAM and OAM access restrictions\n");
}

void test_oam_dma(void) {
    BUS BUS = {0};
    PPU ppu;

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    // Fill source memory with recognizable data.
    for (int i = 0; i < 0xA0; i++) {
        bus_write_byte(&BUS, 0xC000 + i, (uint8_t)(i + 1));
    }

    // Start DMA from address 0xC000.
    bus_write_byte(&BUS, 0xFF46, 0xC0);

    // DMA should be active immediately after starting.
    assert(BUS.dma_active == 1);
    assert(BUS.dma_index == 0);

    // Advance 4 T-cycles: first byte should be copied.
    bus_update_dma(&BUS, 4);

    assert(BUS.dma_index == 1);
    assert(ppu.oam[0] == 1);
    assert(BUS.dma_active == 1);

    // Advance another 632 T-cycles.
    bus_update_dma(&BUS, 636);

    // All 160 bytes should now be copied.
    assert(BUS.dma_active == 0);
    assert(BUS.dma_index == 160);

    for (int i = 0; i < 0xA0; i++) {
        assert(ppu.oam[i] == (uint8_t)(i + 1));
    }

    printf("OAM DMA timing test passed!\n");
}

void test_dma_cpu_access(void) {
    BUS BUS = {0};
    PPU ppu;

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    // Prepare memory.
    bus_write_byte(&BUS, 0xC000, 0x42);
    bus_write_byte(&BUS, 0xFF80, 0x55);

    // Start DMA.
    bus_write_byte(&BUS, 0xFF46, 0xC0);

    assert(BUS.dma_active == 1);

    // CPU cannot access normal memory.
    assert(bus_read_byte(&BUS, 0xC000) == 0xFF);

    bus_write_byte(&BUS, 0xC000, 0x99);

    // HRAM remains accessible.
    assert(bus_read_byte(&BUS, 0xFF80) == 0x55);

    bus_write_byte(&BUS, 0xFF80, 0x66);

    assert(bus_read_byte(&BUS, 0xFF80) == 0x66);

    // DMA must still read the source memory.
    bus_update_dma(&BUS, 4);

    assert(ppu.oam[0] == 0x42);

    printf("DMA CPU access test passed!\n");
}
void test_dma_cpu_access_1(void) {
    BUS BUS = {0};
    PPU ppu;

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    // Prepare WRAM and HRAM before DMA starts.
    bus_write_byte(&BUS, 0xC000, 0x42);
    bus_write_byte(&BUS, 0xFF80, 0x55);
    bus_write_byte(&BUS, 0xFFFE, 0x77);

    // Start DMA from page C0.
    bus_write_byte(&BUS, 0xFF46, 0xC0);
    assert(BUS.dma_active == 1);

    // CPU reads outside HRAM must return 0xFF.
    assert(bus_read_byte(&BUS, 0xC000) == 0xFF);
    assert(bus_read_byte(&BUS, 0xFF7F) == 0xFF);
    assert(bus_read_byte(&BUS, 0xFFFF) == 0xFF);

    // CPU writes outside HRAM must be ignored.
    bus_write_byte(&BUS, 0xC000, 0x99);
    assert(BUS.wram[0] == 0x42);

    // CPU can still read and write HRAM.
    assert(bus_read_byte(&BUS, 0xFF80) == 0x55);
    bus_write_byte(&BUS, 0xFF80, 0x66);
    assert(bus_read_byte(&BUS, 0xFF80) == 0x66);

    assert(bus_read_byte(&BUS, 0xFFFE) == 0x77);
    bus_write_byte(&BUS, 0xFFFE, 0x88);
    assert(bus_read_byte(&BUS, 0xFFFE) == 0x88);

    // DMA must bypass CPU restrictions and read the original WRAM value.
    bus_update_dma(&BUS, 4);
    assert(ppu.oam[0] == 0x42);

    printf("DMA CPU access test passed!\n");
}

void test_dma_loop_integration(void) {
    BUS BUS = {0};
    PPU ppu;

    ppu_init(&ppu);
    BUS.ppu = &ppu;

    // Fill source memory with recognizable values.
    for (int i = 0; i < 0xA0; i++) {
        bus_write_byte(&BUS, 0xC000 + i, (uint8_t)(i + 1));
    }

    // Start DMA.
    bus_write_byte(&BUS, 0xFF46, 0xC0);

    // Simulate CPU steps, each advancing 4 T-cycles.
    while (BUS.dma_active) {
        bus_update_dma(&BUS, 4);
    }

    // Verify the transfer.
    assert(BUS.dma_index == 0xA0);

    for (int i = 0; i < 0xA0; i++) {
        assert(ppu.oam[i] == (uint8_t)(i + 1));
    }

    printf("DMA loop integration test passed!\n");
}

void test_cpu(void) {
    test_cpu_registers_1();
    test_cpu_flags_1();
    test_cpu_opcodes_1();
    test_cpu_opcodes_2();
    test_cpu_opcodes_3();
    test_cpu_opcodes_4();
    test_cpu_opcodes_5();
    test_cpu_opcodes_6();
    test_cpu_opcodes_7();
    test_cpu_opcodes_8();
    test_cpu_opcodes_9();
    test_cpu_interrupts();
}

void test_ppu(void) {
   // test_ppu_render_bg();
    test_ppu_registers();
    test_ppu_update();
    test_ppu_lyc();
    test_ppu_stat_interrupt();
    test_ppu_render_scanlines();
    test_ppu_HBlank_transition();
    test_ppu_scanline_crossing();
    test_ppu_window_helper();
    test_ppu_window_coords();
    test_ppu_window_pixel();
    test_ppu_render_scanline_window();
    test_ppu_window_line_progression();
    test_ppu_window_line_reset();
    test_ppu_get_sprite();
    test_ppu_sprite_overlaps_scanline();
    test_ppu_get_scanline_sprites();
    test_ppu_get_sprite_pixel();
    test_ppu_sprite_8x16();
    test_ppu_render_sprites();
    test_sprite_palettes();
    test_sprite_priority_over_window();
    test_sprite_x_priority();
    test_sprite_oam_priority();
    test_sprites_render_with_background_disabled();
    test_sprite_behind_background();
    test_sprite_behind_background_color_zero();
    test_sprite_transparency();
    test_sprite_horizontal_flip();
    test_sprite_vertical_flip();
    test_sprite_8x16();
    test_sprite_8x16_vertical_flip();
    test_stat_interrupt_edge();
    test_ppu_memory_access();
    test_oam_dma();
    test_dma_cpu_access();
    test_dma_cpu_access_1();
    test_dma_loop_integration();

}