
#ifndef UTFCONVERT_H
#define UTFCONVERT_H

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>

/* Return code if invalid. (xxx_wctomb) */
#define UC_RET_INVALID    -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define UC_RET_TOOSMALL   -2

/** base part */

// return value: input used
int uc_ucs4_from_utf8(const uint8_t *u8, int u8maxlen, uint32_t *pbuf);
int uc_ucs4_from_utf16le(const uint16_t *u16, int u16maxlen, uint32_t *pbuf);

// return value: output used
int uc_ucs4_to_utf8(uint32_t wc, uint8_t *pbuf);
int uc_ucs4_to_utf8_size(uint32_t wc);
int uc_ucs4_to_utf16le(uint32_t wc, uint16_t *pbuf);
int uc_ucs4_to_utf16le_size(uint32_t wc);

int uc_wchar_from_utf8(const uint8_t *u8, int u8maxlen, wchar_t *pbuf, int *pbuf_used);
int uc_wchar_to_utf8(wchar_t *wch, int wchmaxlen, uint8_t *pbuf);

#define uc_utf8ch_size uc_ucs4_to_utf8_size 
#define uc_utf16lech_size uc_ucs4_to_utf16le_size 

/** extension part */

int uc_ucs4str_to_utf8(uint32_t *ucs4str, int ucs4str_len, uint8_t *buf, int buflen);
int uc_ucs4str_to_utf8z(uint32_t *ucs4str, int ucs4str_len, uint8_t *buf, int buflen);
int uc_ucs4str_to_utf8_size(uint32_t *ucs4str, int ucs4str_len);

int uc_ucs4str_to_utf16le(uint32_t *ucs4str, int ucs4str_len, uint16_t *buf, int buflen);
int uc_ucs4str_to_utf16lez(uint32_t *ucs4str, int ucs4str_len, uint16_t *buf, int buflen);
int uc_ucs4str_to_utf16le_size(uint32_t *ucs4str, int ucs4str_len);

int uc_ucs4str_to_wchar(uint32_t *ucs4str, int ucs4str_len, wchar_t *buf, int buflen);
int uc_ucs4str_to_wcharz(uint32_t *ucs4str, int ucs4str_len, wchar_t *buf, int buflen);
int uc_ucs4str_to_wchar_size(uint32_t *ucs4str, int ucs4str_len);

int uc_ucs4str_from_wchar(const wchar_t *wstr, int wstr_len, uint32_t *pbuf, int *pbuf_used);
int uc_ucs4str_from_utf8_size(const uint8_t *u8str, int u8str_len);

#endif
