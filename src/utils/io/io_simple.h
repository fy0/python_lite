
#ifndef PYLITE_UTILS_IO_SIMPLE_H
#define PYLITE_UTILS_IO_SIMPLE_H

#include "io_base.h"

typedef struct {
    int fno;
    int flag;
    pl_uint_t size;
} PyLiteFileSimple;

PyLiteIO PyLiteIOSimple;

#endif
