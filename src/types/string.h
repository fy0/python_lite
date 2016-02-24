
#ifndef PYLITE_TYPES_STRING_H
#define PYLITE_TYPES_STRING_H

#include "object.h"

typedef struct PyLiteStrObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint32_t *ob_val;
} PyLiteStrObject;

uint32_t pylt_obj_str_hash(PyLiteState *state, PyLiteStrObject *obj);
uint32_t pylt_obj_str_forcehash(PyLiteState *state, PyLiteStrObject *obj);
uint32_t pylt_obj_str_eq(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);
uint32_t pylt_obj_str_cmp(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);

PyLiteStrObject* pylt_obj_str_new(PyLiteState *state, const char* str, int size, bool is_raw);

#endif
