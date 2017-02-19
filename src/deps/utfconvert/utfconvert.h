
#ifndef UTFCONVERT_H
#define UTFCONVERT_H

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>
#define MAXUNICODE    0x10FFFF 

/* Return code if invalid. (xxx_wctomb) */
#define UC_RET_INVALID    -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
//#define UC_RET_TOOSMALL   -2

int uc_utf8_to_ucs4(const uint8_t *u8, int u8maxlen, uint32_t *pbuf);
// return value is length of pbuf used
int uc_ucs4_to_utf8(uint32_t wc, uint8_t *pbuf);

int uc_utf16le_to_ucs4(const uint16_t *u16, int u16maxlen, uint32_t *pbuf);
int uc_ucs4_to_utf16le(uint32_t wc, uint16_t *pbuf);

int uc_utf8_to_wchar(const uint8_t *u8, int u8maxlen, wchar_t *pbuf, int *pbuf_used);
int uc_wchar_to_utf8(wchar_t *wch, int wchlen, uint8_t *pbuf);

#endif
