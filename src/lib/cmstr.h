
#ifndef COMPACT_STRING_H
#define COMPACT_STRING_H

#include <stdint.h>

typedef uint8_t  CM_UCS1;
typedef uint16_t CM_UCS2;
typedef uint32_t CM_UCS4;

enum CMEncoding {
    CME_RAW = 0,
    CME_UTF8,
    CME_LATIN1 = sizeof(CM_UCS1),
    CME_UCS2 = sizeof(CM_UCS2),
    CME_UCS4 = sizeof(CM_UCS4),
    CME_UTF8_RAW = 4096,
};

typedef struct CMString {
    enum CMEncoding encoding;
    int length;
    union {
         void *any;
          CM_UCS1 *latin1;
          CM_UCS2 *ucs2;
          CM_UCS4 *ucs4;
    } data;
    struct u8cache {
        uint8_t *str;
        int raw_size;
    } u8cache;
} CMString;

CMString* cmstr_new(void* str, int size, int encoding);
int cmstr_free(CMString* str);

uint32_t cms_at(CMString* str, int index);

const char* utf8_decode(const char *o, int *val);

#endif
