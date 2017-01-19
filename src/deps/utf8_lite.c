
#include "utf8_lite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

const char  abPrefix[] = { 0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
const int adwCodeUp[] = {
    0x80,           // U+00000000 ～ U+0000007F
    0x800,          // U+00000080 ～ U+000007FF
    0x10000,        // U+00000800 ～ U+0000FFFF
    0x200000,       // U+00010000 ～ U+001FFFFF
    0x4000000,      // U+00200000 ～ U+03FFFFFF
    0x80000000      // U+04000000 ～ U+7FFFFFFF
};

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


int utf8str_len(const char *s) {
    int code;
    int len = 0, rlen = strlen(s);
    const char* s_end = s + rlen + 1;

    for (const char *p = utf8_decode(s, &code); p != s_end; p = utf8_decode(p, &code)) {
        len += 1;
    }
    return len;
}

bool ucs4_to_utf8(int code, char *buf, int *plen) {
    int ilen = ucs4_to_utf8_size(code);
    if (!ilen) return false;  // UCS4 编码无效

    if (buf != NULL) {  // 转换为UTF-8编码
        for (int i = ilen - 1; i > 0; --i) {
            buf[i] = (char)((code & 0x3F) | 0x80);
            code >>= 6;
        }
        buf[0] = (char)(code | abPrefix[ilen - 1]);
    }

    if (plen) *plen = ilen;
    return true;
}

int utf8ch_size(const unsigned char c) {
    // <= 0x7f 1
    // >= 0xfc 6  >= 0xf8 5  >= 0xf0 4  >= 0xe0 3  >= 0xc0 2
    uint8_t size_limits[] = { 0xfc, 0xf8, 0xf0, 0xe0, 0xc0 };
    if (c <= 0x7f) return 1;
    for (int i = 0; i < (sizeof(size_limits) / sizeof(uint8_t)); i++) {
        if (c >= size_limits[i]) {
            return 6 - i;
        }
    }
    return 0;
}


int ucs4_to_utf8_size(int code) {
    // 根据UCS4编码范围确定对应的UTF-8编码字节数
    int i;
    int ilen = sizeof(adwCodeUp) / sizeof(uint32_t);
    for (i = 0; i < ilen; ++i) {
        if (code < adwCodeUp[i]) break;
    }
    if (i == ilen) return 0;    // 无效的UCS4编码
    return i + 1;   // UTF-8编码字节数
}

bool ucs4str_to_utf8(uint32_t *ucs4str, int ucs4str_len, char *buf, int *plen) {
    int len, written=0;
    char u8buf[6];

    for (int i = 0; i < ucs4str_len; ++i) {
        if (ucs4_to_utf8(ucs4str[i], (char*)&u8buf, &len)) {
            memcpy(buf + written, &u8buf, len);
            written += len;
        } else return false;
    }

    buf[written] = '\0';
    if (plen) *plen = written;
    return true;
}

int ucs4str_to_utf8_size(uint32_t *ucs4str, int ucs4str_len) {
    int isize, allsize = 0;
    for (int i = 0; i < ucs4str_len; ++i) {
        isize = ucs4_to_utf8_size(ucs4str[i]);
        if (!isize) return 0;
        allsize += isize;
    }
    return allsize;
}


bool ucs4_to_ucs2(uint32_t u4, uint16_t *u2) {
    if (u4 > UINT16_MAX) {
        *u2 = (uint16_t)' ';
        return false;
    }
    *u2 = (uint16_t)u4;
    return true;
}

bool ucs2_to_ucs4(uint16_t u2, uint32_t *u4) {
    *u4 = (uint32_t)u2;
    return true;
}

bool ucs4_to_wchar(uint32_t u4, wchar_t *wch) {
    if (u4 > UINT16_MAX) {
        *wch = (wchar_t)' ';
        return false;
    }
    *wch = (wchar_t)u4;
    return true;
}


bool ucs4str_to_ucs2(uint32_t *ucs4str, int ucs4str_len, uint16_t *buf, bool ignore) {
    for (int i = 0; i < ucs4str_len; ++i) {
        bool ret = ucs4_to_ucs2(ucs4str[i], buf + i);
        if ((!ignore) && (!ret)) return false;
    }
    buf[ucs4str_len] = (uint16_t)'\0';
    return true;
}

bool ucs2str_to_ucs4(uint16_t *ucs2str, int ucs2str_len, uint32_t *buf) {
    for (int i = 0; i < ucs2str_len; ++i) {
        ucs2_to_ucs4(ucs2str[i], buf + i);
    }
    buf[ucs2str_len] = (uint32_t)'\0';
    return true;
}

bool ucs4str_to_wchar(uint32_t *ucs4str, int ucs4str_len, wchar_t *buf, bool ignore) {
    if (sizeof(wchar_t) == sizeof(uint32_t)) {
        memcpy(buf, ucs4str, ucs4str_len * sizeof(uint32_t));
        buf[ucs4str_len] = L'\0';
        return true;
    }

    for (int i = 0; i < ucs4str_len; ++i) {
        bool ret = ucs4_to_wchar(ucs4str[i], buf + i);
        if ((!ignore) && (!ret)) return false;
    }
    buf[ucs4str_len] = L'\0';
    return true;
}

bool wchar_to_ucs4str(wchar_t *wstr, int wstr_len, uint32_t *buf) {
    for (int i = 0; i < wstr_len; ++i) {
        buf[i] = (uint32_t)wstr[i];
    }
    buf[wstr_len] = L'\0';
    return true;
}
