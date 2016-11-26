
#include "khash_obj.h"

void* my_malloc(PyLiteInterpreter *I, size_t size) {
    void* ret = pylt_malloc(I, size);
    memset(ret, 0, size);
    return ret;
}

