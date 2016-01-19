
#include "cmstr.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAXUNICODE    0x10FFFF 

#ifdef _MSC_VER
#include <Windows.h>
#endif

int utf8_char_size(const char *str, int str_size, int* plength);
void* utf8_convert(const uint8_t* u8str, int size, int* pchar_size, int* plength);


CMString* cmstr_new(void* raw_str, int size, int encoding) {
    CMString *str;
    if (encoding < CME_UTF8 || encoding > CME_UCS4) {
        return NULL;
    }

    if (encoding == CME_UTF8) {
        void* data;
        int char_size, str_length;

        data = utf8_convert(raw_str, size, &char_size, &str_length);
        if (!data) return NULL;

        str = malloc(sizeof(CMString));
        str->encoding = char_size;
        str->length = str_length;
        str->data.any = data;

        str->u8cache.str = malloc(size+1);
        str->u8cache.str[size] = '\0';
        str->u8cache.raw_size = size;
        memcpy(str->u8cache.str, raw_str, size);
    } else if (encoding == CME_UTF8_RAW) {
        // just utf-8 encoding, not fixed-length
        int char_size, str_length;
        
        char_size = utf8_char_size(raw_str, size, &str_length);
        if (char_size == -1) return NULL;

        str = malloc(sizeof(CMString));
        str->encoding = CME_UTF8_RAW;
        str->length = str_length;
        str->data.any = NULL;
        
        str->u8cache.str = malloc(size + 1);
        str->u8cache.str[size] = '\0';
        str->u8cache.raw_size = size;
        memcpy(str->u8cache.str, raw_str, size);
    } else if (encoding > CME_UTF8) {
        str = malloc(sizeof(CMString));

        str->encoding = encoding;
        str->length = (int)(size / encoding);
        str->data.any = malloc((size+1) * encoding);
        memcpy(str->data.any, raw_str, size);
        memset((uint8_t*)str->data.any + size, 0, encoding);

        // utf-8 cache will be created when using
        str->u8cache.str = NULL;
        str->u8cache.raw_size = 0;
    }

    return str;
}

int cmstr_free(CMString* str) {
    if (str) {
        free(str->data.any);
        free(str->u8cache.str);
        free(str);
    }
    return 0;
}

/** Get a character by the index of string */
uint32_t cms_at(CMString* str, int index) {
    if (!str || (index > str->length)) return -1;
    switch (str->encoding) {
    case CME_LATIN1:
        return (uint32_t)str->data.latin1[index];
    case CME_UCS2:
        return (uint32_t)str->data.ucs2[index];
    case CME_UCS4:
        return (uint32_t)str->data.ucs4[index];
    }
    return -1;
}

/*
** Decode one UTF-8 sequence, returning NULL if byte sequence is invalid.
*/
const char *utf8_decode(const char *o, int *val) {
    static const unsigned int limits[] = { 0xFF, 0x7F, 0x7FF, 0xFFFF };
    const unsigned char *s = (const unsigned char *)o;
    unsigned int c = s[0];
    unsigned int res = 0;  /* final result */
    if (c < 0x80)  /* ascii? */
        res = c;
    else {
        int count = 0;  /* to count number of continuation bytes */
        while (c & 0x40) {  /* still have continuation bytes? */
            int cc = s[++count];  /* read next byte */
            if ((cc & 0xC0) != 0x80)  /* not a continuation byte? */
                return NULL;  /* invalid byte sequence */
            res = (res << 6) | (cc & 0x3F);  /* add lower 6 bits from cont. byte */
            c <<= 1;  /* to test next bit */
        }
        res |= ((c & 0x7F) << (count * 5));  /* add first byte */
        if (count > 3 || res > MAXUNICODE || res <= limits[count])
            return NULL;  /* invalid byte sequence */
        s += count;  /* skip continuation bytes read */
    }
    if (val) *val = res;
    return (const char *)s + 1;  /* +1 to include first byte */
}


int utf8_char_size(const char *str, int str_size, int* plength) {
    uint8_t c;
    uint8_t size_limits[] = { 0x80, 0xF0, 0xFE };
    const uint8_t *s;
    int i, max_index = -1;
    int length = 0;

    s = str;
    while (true) {
        if (s - str >= str_size) break;
        c = *s;
        if ((c >> 6) != 0x02) {
            length++;
            for (i = 0; i < (sizeof(size_limits) / sizeof(uint8_t)); i++) {
                if (c < size_limits[i]) {
                    if (i > max_index) max_index = i;
                    break;
                }
            }
            if (max_index == -1) return -1;
        }
        s++;
    }

    if (plength) *plength = length;
    switch (max_index) {
    case -1: return -1;
    case 0: return 1;
    case 1: return 2;
    case 2: return 4;
    default: return -1;
    }
}


void* utf8_convert(const uint8_t* u8str, int size, int* pchar_size, int* plength) {
    void* buf;
    const uint8_t *p = u8str;
    int i, code, char_size, str_length;

    char_size = utf8_char_size(u8str, size, &str_length);
    if (char_size == -1) return NULL;

    buf = malloc((str_length + 1) * char_size);
    memset((uint8_t*)buf + (str_length * char_size), 0, char_size);

    if (char_size == 1) {
        memcpy(buf, u8str, str_length);
    } else {
        for (i = 0; i < str_length; i++) {
            p = utf8_decode(p, &code);
            if (!p) {
                free(buf);
                return NULL;
            }
            if (char_size == 2) ((uint16_t*)buf)[i] = (uint16_t)code;
            else ((uint32_t*)buf)[i] = (uint32_t)code;
        }
    }

    if (pchar_size) *pchar_size = char_size;
    if (plength) *plength = str_length;
    return buf;
}


char* ucs4_to_utf8(int code) {
    const char  abPrefix[] = { 0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
    const int adwCodeUp[] = {
        0x80,           // U+00000000 ～ U+0000007F
        0x800,          // U+00000080 ～ U+000007FF
        0x10000,        // U+00000800 ～ U+0000FFFF
        0x200000,       // U+00010000 ～ U+001FFFFF
        0x4000000,      // U+00200000 ～ U+03FFFFFF
        0x80000000      // U+04000000 ～ U+7FFFFFFF
    };

    char* pbUTF8;
    int i, ilen;

    // 根据UCS4编码范围确定对应的UTF-8编码字节数
    ilen = sizeof(adwCodeUp) / sizeof(uint32_t);
    for (i = 0; i < ilen; i++) {
        if (code < adwCodeUp[i]) break;
    }

    if (i == ilen) return NULL;    // 无效的UCS4编码

    ilen = i + 1;   // UTF-8编码字节数
    pbUTF8 = malloc(sizeof(char) * (ilen + 1));

    if (pbUTF8 != NULL) {   // 转换为UTF-8编码
        for (; i > 0; i--) {
            pbUTF8[i] = (char)((code & 0x3F) | 0x80);
            code >>= 6;
        }

        pbUTF8[0] = (char)(code | abPrefix[ilen - 1]);
    }

    pbUTF8[ilen] = 0;

    return pbUTF8;
}
