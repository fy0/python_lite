
#include "config.h"

void* pylt_realloc(PyLiteInterpreter *I, void* m, size_t osize, size_t size) {
    return realloc(m, size);
}
