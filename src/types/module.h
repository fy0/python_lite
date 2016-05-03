
#ifndef PYLITE_TYPES_MODULE_H
#define PYLITE_TYPES_MODULE_H

#include "object.h"
#include "code.h"
#include "dict.h"

typedef struct PyLiteModuleObject {
    PyLiteObject_HEAD;
    PyLiteDictObject *attrs;
    PyLiteCodeObject *code;
} PyLiteModuleObject;

PyLiteModuleObject* pylt_obj_module_new(PyLiteState *state, PyLiteCodeObject *code);
#define pylt_obj_mod_setattr(state, mod, key_str, value) pylt_obj_dict_csetitem(state, (mod)->attrs, castobj(_NS(state, (key_str))), castobj(value));

#endif
