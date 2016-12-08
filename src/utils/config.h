
#ifndef PYLITE_CONFIG_H
#define PYLITE_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef intptr_t pl_int_t;
typedef uintptr_t pl_uint_t;
typedef int32_t pl_int32_t;
typedef uint32_t pl_uint32_t;
typedef double pl_float_t;
typedef bool pl_bool_t;

typedef struct PyLiteInterpreter PyLiteInterpreter;

#define PYLT_LEX_BYTES_DEFAULT_BUFFER_SIZE 64
#define PYLT_LEX_BYTES_DEFAULT_BUFFER_INC_STEP 32
#define PYLT_LEX_STR_DEFAULT_BUFFER_SIZE 24
#define PYLT_LEX_STR_DEFAULT_BUFFER_INC_STEP 24

void* pylt_realloc(PyLiteInterpreter *I, void* m, size_t osize, size_t nsize);
#define pylt_malloc(I, size) pylt_realloc(I, NULL, 0, size)
#define pylt_free(I, m, s) pylt_realloc(I, (m), (s), 0)
#define pylt_free_ex(I, m) pylt_realloc(I, (m), sizeof(*(m)), 0)

#endif
