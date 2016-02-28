
#ifndef PYLITE_TYPES_SET_H
#define PYLITE_TYPES_SET_H

#include "object.h"

KHASHO_INIT(set_obj, PyLiteObject*, char, 0, pylt_obj_chash, pylt_obj_ceq);

typedef struct PyLiteSetObject {
    PyLiteObject_HEAD;
    khasho_t(set_obj) *ob_val;
} PyLiteSetObject;

void pylt_obj_set_add(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj);
PyLiteObject* pylt_obj_set_has(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj);

PyLiteSetObject* pylt_obj_set_new(PyLiteState *state);

#endif
