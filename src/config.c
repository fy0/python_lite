
#include "config.h"

void* pylt_realloc(void* m, size_t size) {
    return realloc(m, size);
}

void pylt_free(void* m) {
    free(m);
}
