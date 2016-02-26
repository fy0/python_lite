
#include "khash_obj.h"

void* my_malloc(size_t size) {
    void* ret = pylt_realloc(NULL, size);
    memset(ret, 0, size);
    return ret;
}

