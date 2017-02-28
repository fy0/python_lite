
#ifndef PYLITE_UTILS_IO_H
#define PYLITE_UTILS_IO_H

#include <stdio.h>
#include "io_base.h"
#include "fs_base.h"
#include "io_simple.h"

PyLiteFile* pylt_io_open(PyLiteInterpreter *I, PyLiteStrObject *fn, int mode, int encoding);
PyLiteFile* pylt_io_open_full(PyLiteInterpreter *I, PyLiteIO *io, PyLiteStrObject *fn, int mode, int encoding);

pl_bool_t pylt_io_readable(PyLiteInterpreter *I, PyLiteFile *pf);
pl_bool_t pylt_io_writeable(PyLiteInterpreter *I, PyLiteFile *pf);
PyLiteStrObject* pylt_io_getencoding(PyLiteInterpreter *I, PyLiteFile *pf);

int pylt_io_read(PyLiteInterpreter *I, PyLiteFile *pf, uint8_t *buf, pl_uint_t count);
int pylt_io_readstr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count);

int pylt_io_write(PyLiteInterpreter *I, PyLiteFile *pf, uint8_t *buf, pl_uint_t count);
int pylt_io_writestr(PyLiteInterpreter *I, PyLiteFile *pf, uint32_t *buf, pl_uint_t count);

int pylt_io_mode_to_posix(int mode);
int pylt_io_mode_parse(PyLiteStrObject *mode, int *perr);

FILE* pylt_io_fopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);

#endif
