
#include "io.h"

FileReader* fr_init(uint8_t *buf, int size) {
    FileReader *fr = pylt_realloc(NULL, sizeof(FileReader));
    fr->buf = buf;
    fr->p = buf;
    fr->size = size;
    return fr;
}

uint32_t fr_getc_u8(FileReader* fr) {
    uint32_t code;
    fr->p = utf8_decode(fr->p, &code);
    return code;
}

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
