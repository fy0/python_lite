// 不应被任何头文件所引用，从而暴露给外部

#ifndef PYLITE_MISC_H
#define PYLITE_MISC_H

#include "../deps/kvec.h"
#include "../deps/platform.h"
#include "../deps/utfconvert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>

#ifndef PLATFORM_WINDOWS
#ifndef O_BINARY
#define O_BINARY  0
#endif
#endif

#include "config.h"
#include "static.h"

typedef struct PyLiteStrObject PyLiteStrObject;

typedef struct RawString {
    const uint8_t *s;
    const uint8_t *e;
} RawString;

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

#define swap(a, b, _type) { _type tmp = a; a = b; b = tmp; }
#define clamp(val, imin, imax) min(max(val, imin), imax) // 这是最优的吗？我不知道

#define kv_pushptr(v, x) kv_push(uintptr_t, (v), (uintptr_t)(x))

#endif
