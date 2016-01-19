
#ifndef PYLITE_IO_H
#define PYLITE_IO_H

#include "utils.h"

typedef struct pylt_FileSystem {
    void(*fs_init);
    int(*fs_exists);
    void(*fs_final);
} pylt_FileSystem;

typedef struct FileReader {
    uint8_t *buf;
    uint8_t *p;
    int size;
} FileReader;

FileReader* fr_init(uint8_t *buf, int size);
uint32_t fr_getc_u8(FileReader* fr);

char* read_file(const char* fn, int *psize);

#endif
