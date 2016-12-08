
#include "config.h"
#include "../intp.h"

void* pylt_realloc(PyLiteInterpreter *I, void* m, size_t osize, size_t nsize) {
    if (nsize == 0) {
        free(m);
        if (I) I->mem_used -= osize;
        return NULL;
    } else {
        void* ret = realloc(m, nsize);
        if (I) I->mem_used = I->mem_used - osize + nsize;
        return ret;
    }
}
