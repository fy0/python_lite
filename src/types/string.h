
#ifndef PYLITE_TYPES_STRING_H
#define PYLITE_TYPES_STRING_H

#include "object.h"

typedef struct PyLiteStrObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint32_t *ob_val;
} PyLiteStrObject;

pl_int_t pylt_obj_str_ccmp(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_str_ceq(PyLiteState *state, PyLiteStrObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_str_chash(PyLiteState *state, PyLiteStrObject *obj);
pl_uint32_t pylt_obj_str_cforcehash(PyLiteState *state, PyLiteStrObject *obj);

PyLiteStrObject* pylt_obj_str_new(PyLiteState *state, uint32_t* str, int size, bool is_raw);
PyLiteStrObject* pylt_obj_str_new_from_c_str(PyLiteState *state, char* str, bool is_raw);
void pylt_obj_str_free(PyLiteState *state, PyLiteStrObject *self);

#endif
