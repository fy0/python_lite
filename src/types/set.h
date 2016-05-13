
#ifndef PYLITE_TYPES_SET_H
#define PYLITE_TYPES_SET_H

#include "object.h"

KHASHO_INIT(set_obj, PyLiteObject*, char, 0, pylt_obj_hash, pylt_obj_eq);

typedef struct PyLiteSetObject {
    PyLiteObject_HEAD;
    khasho_t(set_obj) *ob_val;
} PyLiteSetObject;

pl_int_t pylt_obj_set_cmp(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_set_eq(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *other);

pl_int_t pylt_obj_set_len(PyLiteState *state, PyLiteSetObject *self);
pl_int_t pylt_obj_set_add(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj);
void pylt_obj_set_clear(PyLiteState *state, PyLiteSetObject *self);
PyLiteSetObject* pylt_obj_set_copy(PyLiteState *state, PyLiteSetObject *self);
pl_bool_t pylt_obj_set_contains(PyLiteState *state, PyLiteSetObject *self, PyLiteSetObject *other);
PyLiteObject* pylt_obj_set_has(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj);

pl_int_t pylt_obj_set_remove(PyLiteState *state, PyLiteSetObject *self, PyLiteObject *obj);
PyLiteObject* pylt_obj_set_pop(PyLiteState *state, PyLiteSetObject *self);

pl_int_t pylt_obj_set_begin(PyLiteState *state, PyLiteSetObject *self);
pl_int_t pylt_obj_set_end(PyLiteState *state, PyLiteSetObject *self);
void pylt_obj_set_next(PyLiteState *state, PyLiteSetObject *self, pl_int_t *k);

// 存在则取值 否则为空
PyLiteObject* pylt_obj_set_itemvalue(PyLiteState *state, PyLiteSetObject *self, pl_int_t k);

PyLiteSetObject* pylt_obj_set_new(PyLiteState *state);

#endif
