
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "../object.h"
#include "../common/string.h"
#include "../common/dict.h"
#include "code.h"

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteDictObject *ob_attrs;
    PyLiteCodeObject *code;

    PyLiteStrObject *name; // module name
    PyLiteObject *ob_owner; // owner of object
    // __file__, etc.
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new_ex(PyLiteInterpreter *I, PyLiteObject *owner, PyLiteStrObject *name);
PyLiteModuleObject* pylt_obj_module_new(PyLiteInterpreter *I, PyLiteStrObject *name);
void pylt_obj_mod_setattr(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *key, PyLiteObject *value);
PyLiteObject* pylt_obj_mod_getattr(PyLiteInterpreter *I, PyLiteModuleObject *self, PyLiteStrObject *key);
void pylt_obj_module_rfree(PyLiteInterpreter *I, PyLiteModuleObject *self);
void pylt_obj_module_free(PyLiteInterpreter *I, PyLiteModuleObject *self);

#endif
