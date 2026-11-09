//
// Created by Livia on 09.11.2026.
//

#include "save.h"

#include "save.h"
#include <stdio.h>
#include <string.h>

int save_load(BUS *BUS, const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
        return 0;

    size_t bytes_read = fread(
        BUS->cart_ram,
        1,
        sizeof(BUS->cart_ram),
        file
    );

    fclose(file);

    return bytes_read == sizeof(BUS->cart_ram);
}

int save_write(BUS *BUS, const char *filename)
{
    FILE *file = fopen(filename, "wb");

    if (file == NULL)
        return 0;

    size_t bytes_written = fwrite(
        BUS->cart_ram,
        1,
        sizeof(BUS->cart_ram),
        file
    );

    fclose(file);

    return bytes_written == sizeof(BUS->cart_ram);
}