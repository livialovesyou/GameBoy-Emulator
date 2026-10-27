#include <stdio.h>
#include <stdint.h>
#include "bus.h"
#include "test.h"
#include "cpu.h"



int main(void) {
    CPU cpu = {0};
    BUS BUS = {0};

    test_cpu();
    // test_cpu_opcodes_1();
    // test_cpu_opcodes_2();
    // test_cpu_opcodes_3();
    // test_cpu_opcodes_4();
    // test_cpu_opcodes_5();
    // test_cpu_opcodes_6();
    // test_cpu_opcodes_7();
    // test_cpu_opcodes_8();
    // test_cpu_opcodes_9();
    // test_cpu_interrupts();
    //test_cpu_flags_1();
    //test_cpu_registers_1();
    //echo_ram_test_1();
    // test_unimplemented_address();
    return 0;
}
