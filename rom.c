//
// Created by Livia on 06.11.2026.
//

#include "rom.h"

#include <stdio.h>


int rom_load(const char *filename, uint8_t rom[ROM_SIZE]) {

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    size_t bytes_read = fread(rom, 1, size, file);

    if (ferror(file)) {
        fprintf(stderr, "Error reading %s\n", filename);
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

