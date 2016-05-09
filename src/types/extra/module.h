
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "../object.h"
#include "../string.h"
#include "../dict.h"
#include "code.h"

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteDictObject *attrs;
    PyLiteCodeObject *code;
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeObject *code);
void pylt_obj_mod_setattr(PyLiteState *state, PyLiteModuleObject *mod, PyLiteStrObject *key, PyLiteObject *value);
PyLiteObject* pylt_obj_mod_getattr(PyLiteState *state, PyLiteModuleObject *self, PyLiteObject *key);

#endif
