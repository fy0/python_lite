
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

PyLiteModuleObject* pylt_obj_module_new(PyLiteInterpreter *I, PyLiteCodeObject *code);
void pylt_obj_mod_setattr(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *key, PyLiteObject *value);
PyLiteObject* pylt_obj_mod_getattr(PyLiteInterpreter *I, PyLiteModuleObject *self, PyLiteObject *key);
void pylt_obj_module_free(PyLiteInterpreter *I, PyLiteModuleObject *self);

#endif
