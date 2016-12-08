
#include "misc.h"
#include "../intp.h"
#include "../types/all.h"

void putcode(uint32_t code) {
    if (code < 0xff) {
        putchar((char)code);
    } else {
        int len;
        char buf[7];
        if (ucs4_to_utf8(code, (char*)&buf, &len)) {
            buf[len] = '\0';
            printf_u8("%s", &buf);
        }
    }
}

void raw_str_print(RawString *rs) {
    uint32_t code;
    for (const uint8_t *p = rs->s; p != rs->e;) {
        p = utf8_decode(p, &code);
        putcode(code);
    }
}
