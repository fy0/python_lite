
#ifndef PYLITE_CORE_IO_H
#define PYLITE_CORE_IO_H

#include "misc.h"

enum PYLT_IO_TEXT_ENCODING {
    PYLT_IOTE_BYTE = 0,
    PYLT_IOTE_UTF8 = 1,
    PYLT_IOTE_UCS2 = 2,
    PYLT_IOTE_UCS4 = 4,
    PYLT_IOTE_WCHAR = 8
};


typedef struct {
    int fno;
    int flags;
    int encoding;
    pl_bool_t is_binary;
    pl_uint_t current;
    pl_uint_t size;
} PyLiteFile;


FILE* pylt_io_fopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);
PyLiteFile* pylt_io_file_new(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode, int encoding);
PyLiteFile* pylt_io_file_new_with_cfile(PyLiteInterpreter *I, FILE *fp, int encoding);

PyLiteFile* pylt_io_file_new_ex(PyLiteInterpreter *I, char *fn, char *mode, int encoding);

int pylt_io_file_read(PyLiteInterpreter *I, PyLiteFile *pf, void *buf, pl_uint_t count);
int pylt_io_file_readstr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count);

int pylt_io_file_write(PyLiteInterpreter *I, PyLiteFile *pf, void *buf, pl_uint_t count);
int pylt_io_file_writestr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count, uint32_t ignore);

pl_bool_t pylt_io_file_writeable(PyLiteInterpreter *I, PyLiteFile *pf);
pl_bool_t pylt_io_file_readable(PyLiteInterpreter *I, PyLiteFile *pf);

void pylt_io_init(PyLiteInterpreter *I);

#endif
