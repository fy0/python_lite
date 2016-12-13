// ��Ӧ���κ�ͷ�ļ������ã��Ӷ���¶���ⲿ

#ifndef PYLITE_MISC_H
#define PYLITE_MISC_H

#include "../deps/kvec.h"
#include "../deps/platform.h"
#include "../deps/utf8_lite.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#ifdef PLATFORM_WINDOWS
#include <FCNTL.H>
#else
#include <fcntl.h>
#endif

#include "config.h"
#include "static.h"

typedef struct PyLiteStrObject PyLiteStrObject;

typedef struct RawString {
    const uint8_t *s;
    const uint8_t *e;
} RawString;

void putcode(uint32_t code);
void raw_str_print(RawString *rs);

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define swap(a, b, _type) { _type tmp = a; a = b; b = tmp; }
#define clamp(val, imin, imax) min(max(val, imin), imax) // �������ŵ����Ҳ�֪��

#define kv_pushptr(v, x) kv_push(uintptr_t, (v), (uintptr_t)(x))

#endif
