
#ifndef UTF8_LITE_H
#define UTF8_LITE_H

#include <stdint.h>
#include <stdbool.h>
#define MAXUNICODE    0x10FFFF 

const char *utf8_decode (const char *o, int *val);
int utf8str_len(const char *s);

// ת��ucs4->utf8
bool ucs4_to_utf8(int code, char *buf, int *plen);
// ����utf8�ִ�ռ�õĿռ�
int ucs4_to_utf8_size(int code);
// ����ucs4�ַ���->utf8�ַ�����ת��
bool ucs4str_to_utf8(uint32_t *ucs4str, int ucs4str_len, char *buf, int *plen);
// ת��ǰ����utf8�ִ�ռ�õĿռ�
int ucs4str_to_utf8_size(uint32_t *ucs4str, int ucs4str_len);

#endif
