
#include "io.h"


char* read_file(const char* fn, int *psize) {
    int size;
    char* buf;

    FILE *fp = fopen(fn, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buf = pylt_realloc(NULL, size + 1);
        buf[size] = '\0';
        fread(buf, size, 1, fp);
        fclose(fp);
        *psize = size;
        return buf;
    }
    return NULL;
}


StringStream* ss_new(uint8_t *buf, int size) {
    StringStream *ss = pylt_realloc(NULL, sizeof(StringStream));
    ss->buf = buf;
    ss->p = buf;
    ss->now = 0;
    ss->size = size;
    ss->page = 0;
    return ss;
}

void ss_free(StringStream *ss) {
    pylt_free(ss);
}

void ss_nextc(StringStream* ss) {
    ss->now++;
    ss->p = utf8_decode(ss->p, &(ss->current));
}

/** 不做边界检查 用户应保证边界正确 */
uint32_t ss_lastc(StringStream *ss) {
    // TODO: utf-8
    const uint8_t *p = ss->p - 1;
    return *p;
}

const uint8_t* ss_lastpos(StringStream *ss) {
    const uint8_t *p = ss->p - 1;
    return p;
}

void ss_savepos(StringStream *ss, StringStreamSave *save) {
    save->pos = ss->p;
    save->current = ss->current;
}

void ss_loadpos(StringStream* ss, StringStreamSave *save) {
    ss->p = save->pos;
    ss->current = save->current;
}
