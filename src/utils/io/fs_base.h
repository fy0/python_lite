
#ifndef PYLITE_UTILS_FS_BASE_H
#define PYLITE_UTILS_FS_BASE_H

#include "../../types/all.h"

typedef pl_bool_t(*PyLiteFSFileExistsFunc)(PyLiteInterpreter *I, PyLiteStrObject *fn);
typedef pl_bool_t(*PyLiteFSIsDirFunc)(PyLiteInterpreter *I, PyLiteStrObject *fn);

typedef struct PyLiteFS {
    PyLiteFSFileExistsFunc exists;
    PyLiteFSIsDirFunc isdir;
} PyLiteFS;

#endif
