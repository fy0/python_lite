
#ifndef UTF8_LITE_H
#define UTF8_LITE_H

#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>
#define MAXUNICODE    0x10FFFF 

const char *utf8_decode (const char *o, int *val);
int utf8str_len(const char *s);

int utf8ch_size(const char c);

// 转换字符 ucs4 -> utf8
bool ucs4_to_utf8(int code, char *buf, int *plen);
// 计算utf8字串占用的空间
int ucs4_to_utf8_size(int code);
// 转换字符串 ucs4 -> utf8 (末尾补\0)
bool ucs4str_to_utf8(uint32_t *ucs4str, int ucs4str_len, char *buf, int *plen);
// 转换前计算utf8字串占用的空间
int ucs4str_to_utf8_size(uint32_t *ucs4str, int ucs4str_len);

// 转换字符 ucs4 -> ucs2
bool ucs4_to_ucs2(uint32_t u4, uint16_t *u2);
// 转换字符 ucs2 -> ucs4
bool ucs2_to_ucs4(uint16_t u2, uint32_t *u4);
// 转换字符 ucs4 -> wchar_t
bool ucs4_to_wchar(uint32_t u4, wchar_t *wch);
// 转换字符串 ucs4 -> ucs2 (末尾补\0)
bool ucs4str_to_ucs2(uint32_t *ucs4str, int ucs4str_len, uint16_t *buf, bool ignore);
// 转换字符串 ucs2 -> ucs4 (末尾补\0)
bool ucs2str_to_ucs4(uint16_t *ucs2str, int ucs2str_len, uint32_t *buf);
// 转换字符串 ucs4 -> wchar_t (末尾补\0)
bool ucs4str_to_wchar(uint32_t *ucs4str, int ucs4str_len, wchar_t *buf, bool ignore);

#endif
