
#ifndef PYLITE_IO_H
#define PYLITE_IO_H

#include "utils/config.h"

typedef struct pylt_FileSystem {
    void(*fs_init);
    int(*fs_exists);
    void(*fs_final);
} pylt_FileSystem;

char* read_file(const char* fn, int *psize);


typedef struct StringStream {
    const uint8_t *buf;
    const uint8_t *p;

    int now;
    int size;
    int page;
    uint32_t current;
} StringStream;

typedef struct StringStreamSave {
    const uint8_t *pos;
    uint32_t current;
} StringStreamSave;

StringStream* ss_new(uint8_t *buf, int size);
void ss_free(StringStream *ss);
void ss_nextc(StringStream *ss);
uint32_t ss_lastc(StringStream *ss);
const uint8_t* ss_lastpos(StringStream *ss);

void ss_savepos(StringStream *ss, StringStreamSave *save);
void ss_loadpos(StringStream* ss, StringStreamSave *save);
//void ss_moveback();
//void ss_curpos();
//void ss_new_from_file(uint8_t *buf);

#endif
