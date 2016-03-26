
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "object.h"
#include "codesnippet.h"

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteTable *attrs;
    PyLiteCodeSnippet code;
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeSnippet *code);

#endif
