
#ifndef PYLITE_CORE_IO_H
#define PYLITE_CORE_IO_H

#include "misc.h"

enum PYLT_IO_TEXT_ENCODING {
    PYLT_IOTE_BYTE = 0,
    PYLT_IOTE_UTF8,
    PYLT_IOTE_UCS4,
};


typedef struct {
    int fno;
    int flags;
    int encoding;
    pl_uint_t current;
    pl_uint_t size;
} PyLiteFile;


FILE* pl_io_fopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);
PyLiteFile* pl_io_file_new(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);
PyLiteFile* pl_io_file_new_with_cfile(PyLiteInterpreter *I, FILE *fp);

int pl_io_file_read(PyLiteInterpreter *I, PyLiteFile *pf, void *buf, pl_uint_t count);
int pl_io_file_readstr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count);
void pl_io_init(PyLiteInterpreter *I);

#endif
