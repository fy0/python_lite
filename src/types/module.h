
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "object.h"
#include "codesnippet.h"

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteTable *attrs;
    PyLiteCodeSnippetObject code;
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeSnippetObject *code);

#endif
