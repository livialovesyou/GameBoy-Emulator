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