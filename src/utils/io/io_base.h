
#ifndef PYLITE_UTILS_IO_BASE_H
#define PYLITE_UTILS_IO_BASE_H

#include "../../types/all.h"

enum PYLT_IO_TEXT_ENCODING {
    PYLT_IOTE_UTF8  = 1,
    PYLT_IOTE_UTF16 = 2,
    PYLT_IOTE_UCS4  = 4,
    PYLT_IOTE_WCHAR = 8
};

enum PYLT_IO_MODE {
    PYLT_IOMODE_READ = 1,
    PYLT_IOMODE_WRITE = 2,
    PYLT_IOMODE_APPEND = 4,
    PYLT_IOMODE_CREATE = 8, // EXCL

    PYLT_IOMODE_PLUS = 16,
    PYLT_IOMODE_BINARY = 32,
};

typedef struct PyLiteIO;

typedef struct {
    void *data;
    int mode;
    int encoding;
    struct PyLiteIO *io;
} PyLiteFile;

typedef void* (*PyLiteIOFileOpenFunc)(PyLiteInterpreter *I, PyLiteStrObject *fn, int openmode);
typedef pl_int_t(*PyLiteIOFileCloseFunc)(PyLiteInterpreter *I, void *pfdata);
typedef pl_int_t(*PyLiteIOReadFunc)(PyLiteInterpreter *I, void *pfdata, uint8_t *buf, pl_uint_t count);
typedef pl_int_t(*PyLiteIOWriteFunc)(PyLiteInterpreter *I, void *pfdata, uint8_t *buf, pl_uint_t count);

typedef struct PyLiteIO {
    PyLiteIOFileOpenFunc open;
    PyLiteIOFileCloseFunc close;
    PyLiteIOReadFunc read;
    PyLiteIOWriteFunc write;
} PyLiteIO;

#endif
