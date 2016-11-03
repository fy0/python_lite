
#ifndef PYLITE_MODS_CIO_H
#define PYLITE_MODS_CIO_H

#include "../types/all.h"

FILE* mfopen(PyLiteInterpreter *I, PyLiteStrObject *fn, PyLiteStrObject *mode);
PyLiteModuleObject* pylt_mods_cio_register(PyLiteInterpreter *I);

#endif
