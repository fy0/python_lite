
#ifndef PYLITE_CONFIG_H
#define PYLITE_CONFIG_H

#include "lib/platform.h"
#include "lib/utf8_lite.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

typedef intptr_t pl_int_t;
typedef uintptr_t pl_uint_t;
typedef int32_t pl_int32_t;
typedef uint32_t pl_uint32_t;
typedef double pl_float_t;
typedef bool pl_bool_t;

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

typedef struct PyLiteInterpreter PyLiteInterpreter;

#define PYLT_LEX_BYTES_DEFAULT_BUFFER_SIZE 64
#define PYLT_LEX_BYTES_DEFAULT_BUFFER_INC_STEP 32
#define PYLT_LEX_STR_DEFAULT_BUFFER_SIZE 24
#define PYLT_LEX_STR_DEFAULT_BUFFER_INC_STEP 24

void* pylt_realloc(void* m, size_t size);
void pylt_free(void* m);

#endif

