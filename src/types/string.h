
#ifndef PYLITE_TYPES_STRING_H
#define PYLITE_TYPES_STRING_H

#include "object.h"

typedef struct PyLiteStrObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint32_t *ob_val;
} PyLiteStrObject;

pl_int_t pylt_obj_str_cmp(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_str_eq(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_str_hash(PyLiteState *state, PyLiteStrObject *obj);
pl_uint32_t pylt_obj_str_forcehash(PyLiteState *state, PyLiteStrObject *obj);

PyLiteStrObject* pylt_obj_str_getitem(PyLiteState *state, PyLiteStrObject *self, int index);

PyLiteObject* pylt_obj_str_mul(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_str_plus(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);

pl_int_t pylt_obj_str_index(PyLiteState *state, PyLiteStrObject *self, PyLiteStrObject *sub);
pl_int_t pylt_obj_str_index_full(PyLiteState *state, PyLiteStrObject *self, PyLiteStrObject *sub, pl_int_t start, pl_int_t end);

PyLiteStrObject* pylt_obj_str_new(PyLiteState *state, uint32_t* str, int size, bool is_raw);
PyLiteStrObject* pylt_obj_str_new_from_c_str(PyLiteState *state, char* str, bool is_raw);
PyLiteStrObject* pylt_obj_str_new_empty(PyLiteState *state);
void pylt_obj_str_free(PyLiteState *state, PyLiteStrObject *self);
void pylt_obj_str_safefree(PyLiteState *state, PyLiteStrObject *self);

#endif
