
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "object.h"
#include "../lib/kvec.h"

typedef struct PyLiteCodeSnippet {
    kvec_t(PyLiteObject*) const_val;
    kvec_t(uintptr_t) opcodes;
} PyLiteCodeSnippet;

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteTable *attrs;
    kvec_t(PyLiteObject*) const_val;
    kvec_t(uintptr_t) opcodes;
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeSnippet *code);

#endif
