
#include "UTFConvert.h"

int uc_utf8_to_ucs4(const uint8_t *u8, int u8maxlen, uint32_t *pbuf) {
    static const unsigned int limits[] = { 0xFF, 0x7F, 0x7FF, 0xFFFF };
    const unsigned char *s = (const unsigned char *)u8;
    unsigned int c = s[0];
    unsigned int res = 0;  /* final result */
    if (c < 0x80)  /* ascii? */
        res = c;
    else {
        int count = 0;  /* to count number of continuation bytes */
        while (c & 0x40) {  /* still have continuation bytes? */
            if (u8maxlen && (count == u8maxlen)) return UC_RET_INVALID;
            int cc = s[++count];  /* read next byte */
            if ((cc & 0xC0) != 0x80)  /* not a continuation byte? */
                return UC_RET_INVALID;  /* invalid byte sequence */
            res = (res << 6) | (cc & 0x3F);  /* add lower 6 bits from cont. byte */
            c <<= 1;  /* to test next bit */
        }
        res |= ((c & 0x7F) << (count * 5));  /* add first byte */
        if (count > 3 || res > MAXUNICODE || res <= limits[count])
            return UC_RET_INVALID;  /* invalid byte sequence */
        s += count;  /* skip continuation bytes read */
    }
    if (pbuf) *pbuf = res;
    return (s - u8) + 1;  /* +1 to include first byte */
}

int uc_ucs4_to_utf8(uint32_t wc, uint8_t *pbuf) /* n == 0 is acceptable */ {
    int count;
    if (wc < 0x80) count = 1;
    else if (wc < 0x800) count = 2;
    else if (wc < 0x10000) count = 3;
    else if (wc < 0x200000) count = 4;
    else if (wc < 0x4000000) count = 5;
    else if (wc <= 0x7fffffff) count = 6;
    else return UC_RET_INVALID;

    switch (count) { /* note: code falls through cases! */
        case 6: pbuf[5] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x4000000;
        case 5: pbuf[4] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x200000;
        case 4: pbuf[3] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x10000;
        case 3: pbuf[2] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x800;
        case 2: pbuf[1] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0xc0;
        case 1: pbuf[0] = wc;
    }
    return count;
}

int uc_utf16le_to_ucs4(const uint16_t *u16, int u16maxlen, uint32_t *pbuf) {
    uint16_t w1 = u16[0];
    if (w1 >= 0xD800 && w1 <= 0xDFFF) {
        if (w1 < 0xDC00) {
            if (u16maxlen) return UC_RET_INVALID;
            uint16_t w2 = u16[1];
            if (w2 >= 0xDC00 && w2 <= 0xDFFF) {
                if (pbuf) *pbuf = (uint32_t)(w2 & 0x03FF) + (((w1 & 0x03FF) + 0x40) << 10);
                return 2;
            }
        }
        return UC_RET_INVALID;
    } else {
        if (pbuf) *pbuf = (uint32_t)w1;
        return 1;
    }
}

int uc_ucs4_to_utf16le(uint32_t wc, uint16_t *pbuf) {
    uint8_t *pb8 = (uint8_t *)pbuf;
    if (!(wc >= 0xd800 && wc < 0xe000)) {
        if (wc < 0x10000) {
            pb8[0] = (uint8_t)wc;
            pb8[1] = (uint8_t)(wc >> 8);
            return 1;
        } else if (wc < 0x110000) {
            uint32_t wc1 = 0xd800 + ((wc - 0x10000) >> 10);
            uint32_t wc2 = 0xdc00 + ((wc - 0x10000) & 0x3ff);
            pb8[0] = (uint8_t)wc1;
            pb8[1] = (uint8_t)(wc1 >> 8);
            pb8[2] = (uint8_t)wc2;
            pb8[3] = (uint8_t)(wc2 >> 8);
            return 2;
        }
    }
    return UC_RET_INVALID;
}

int uc_utf8_to_wchar(const uint8_t *u8, int u8maxlen, wchar_t *pbuf, int *pbuf_used) {
    if (sizeof(wchar_t) == sizeof(uint16_t)) {
        uint32_t tmp;
        int ret = uc_utf8_to_ucs4(u8, u8maxlen, &tmp);
        if (ret <= 0) return ret;
        *pbuf_used = uc_ucs4_to_utf16le(tmp, (uint16_t*)pbuf);
        return ret;
    } else if (sizeof(wchar_t) == sizeof(uint32_t)) {
        *pbuf_used = 1;
        return uc_utf8_to_ucs4(u8, u8maxlen, (uint32_t*)pbuf);
    }
    return UC_RET_INVALID;
}

int uc_wchar_to_utf8(wchar_t *wch, int wchlen, uint8_t *pbuf) {
    if (sizeof(wchar_t) == sizeof(uint16_t)) {
        uint32_t tmp;
        int ret = uc_utf16le_to_ucs4((const uint16_t*)wch, wchlen, &tmp);
        if (ret <= 0) return ret;
        return uc_ucs4_to_utf8(tmp, pbuf);
    } else if (sizeof(wchar_t) == sizeof(uint32_t)) {
        return uc_ucs4_to_utf8((uint32_t)*wch, pbuf);
    }
    return UC_RET_INVALID;
}
