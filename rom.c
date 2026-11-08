//
// Created by Livia on 06.11.2026.
//

#include "rom.h"

#include <stdio.h>
#include <stdlib.h>


int rom_load(const char *filename, uint8_t **rom, size_t *rom_size) {

   *rom = NULL;
    *rom_size = 0;
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Could not open file %s\n", filename);
        return 0;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return 0;
    }

    long file_size = ftell(f);
    if (file_size <= 0) {
        fprintf(stderr, "Could not read file %s\n", filename);
        fclose(f);
        return 0;
    }
    rewind(f);

    uint8_t *buffer = malloc((size_t)file_size);

    if (buffer == NULL) {
        fprintf(stderr, "Could not allocate file %s\n", filename);
        fclose(f);
        return 0;
    }

    size_t bytes_read = fread(buffer, 1, (size_t)file_size, f);

    if (bytes_read != (size_t)file_size || ferror(f)) {
        fprintf(stderr, "Could not read file %s\n", filename);
        free(buffer);
        fclose(f);
        return 0;
    }
    fclose(f);

    *rom = buffer;
    *rom_size = (size_t)file_size;

    return 1;
}

