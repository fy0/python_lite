
#ifndef PYLITE_REF_H
#define PYLITE_REF_H

#include "../types/object.h"

typedef struct PyLiteRef {
    PyLiteObject *obj;
} PyLiteRef;

PyLiteRef* pylt_ref_new(PyLiteInterpreter *I, PyLiteObject *obj);
void pylt_ref_free(PyLiteInterpreter *I, PyLiteRef *ref);

#endif
