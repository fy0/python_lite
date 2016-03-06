
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "object.h"

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteTable *attrs;
    PyLiteObject *main_func;
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteObject *main_func);

#endif
