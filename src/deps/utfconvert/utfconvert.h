
#ifndef UTFCONVERT_H
#define UTFCONVERT_H

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>
#define MAXUNICODE    0x10FFFF 

/* Return code if invalid. (xxx_wctomb) */
#define UC_RET_INVALID    -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define UC_RET_TOOSMALL   -2

/* Return code if invalid input after a shift sequence of n bytes was read.
(xxx_mbtowc) */
#define UC_RET_SHIFT_ILSEQ(n)  (-1-2*(n))
/* Return code if invalid. (xxx_mbtowc) */
#define UC_RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define UC_RET_TOOFEW(n)       (-2-2*(n))


// return value is length of ostr read
int uc_utf8_to_ucs4(const uint8_t *ostr, uint32_t *pbuf);
// return value is length of pbuf used
int uc_ucs4_to_utf8(uint32_t wc, uint8_t *pbuf);

int uc_utf16le_to_ucs4(const uint16_t *u16, uint32_t *pbuf);
int uc_ucs4_to_utf16le(uint32_t wc, uint16_t *pbuf);

int uc_utf8_to_wchar(const uint8_t *ostr, wchar_t *pbuf);
int uc_wchar_to_utf8(wchar_t wc, uint8_t *pbuf);

#endif
