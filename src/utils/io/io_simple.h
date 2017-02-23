
#ifndef PYLITE_UTILS_IO_SIMPLE_H
#define PYLITE_UTILS_IO_SIMPLE_H

#include "io_base.h"

typedef struct {
    int fno;
    int flag;
    pl_uint_t size;
} PyLiteFileSimple;

PyLiteIO PyLiteIOSimple;

#define pylt_io_simple_openRead(I, fn) \
    pylt_io_open(I, &PyLiteIOSimple, fn, PYLT_IOMODE_READ, PYLT_IOTE_UTF8)

#endif
