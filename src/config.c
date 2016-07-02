
#include "config.h"

void* pylt_realloc(void* m, size_t size) {
    return realloc(m, size);
}

void pylt_free(void* m) {
    free(m);
}

void putcode(uint32_t code) {
    if (code < 0xff) {
        putchar((char)code);
    } else {
        char* ret = ucs4_to_utf8(code);
        printf_u8("%s", ret);
        free(ret);
    }
}

void raw_str_print(RawString *rs) {
    uint32_t code;
    for (const uint8_t *p = rs->s; p != rs->e;) {
        p = utf8_decode(p, &code);
        putcode(code);
    }
}
