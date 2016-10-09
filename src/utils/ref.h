
#ifndef PYLITE_REF_H
#define PYLITE_REF_H

#include "../intp.h"
#include "../types/object.h"

typedef struct PyLiteRef {
    PyLiteObject *obj;
} PyLiteRef;

PyLiteRef* pl_ref_new(PyLiteInterpreter *I, PyLiteObject *obj);
void pl_ref_free(PyLiteInterpreter *I, PyLiteRef *ref);

#endif
