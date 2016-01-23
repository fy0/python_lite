
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
    ss->size = size;
    return ss;
}

void ss_free(StringStream *ss) {
    pylt_free(ss);
}

uint32_t ss_nextc(StringStream* ss) {
    uint32_t code;
    ss->p = utf8_decode(ss->p, &code);
    return code;
}

void ss_savepos(StringStream *ss, StringStreamSave *save, uint32_t current) {
    save->pos = ss->p;
    save->current = current;
}

uint32_t ss_loadpos(StringStream* ss, StringStreamSave *save) {
    ss->p = save->pos;
    return save->current;
}
