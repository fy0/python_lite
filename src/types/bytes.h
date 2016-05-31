
#ifndef PYLITE_TYPES_BYTES_H
#define PYLITE_TYPES_BYTES_H

#include "object.h"

typedef struct PyLiteBytesObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint8_t *ob_val;
} PyLiteBytesObject;

pl_int_t pylt_obj_bytes_cmp(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_bytes_eq(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_bytes_hash(PyLiteState *state, PyLiteBytesObject *obj);
pl_uint32_t pylt_obj_bytes_forcehash(PyLiteState *state, PyLiteBytesObject *obj);

PyLiteBytesObject* pylt_obj_bytes_getitem(PyLiteState *state, PyLiteBytesObject *self, int index);

PyLiteObject* pylt_obj_bytes_mul(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_bytes_plus(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);

pl_int_t pylt_obj_bytes_index(PyLiteState *state, PyLiteBytesObject *self, PyLiteBytesObject *sub);
pl_int_t pylt_obj_bytes_index_full(PyLiteState *state, PyLiteBytesObject *self, PyLiteBytesObject *sub, pl_int_t start, pl_int_t end);

PyLiteBytesObject* pylt_obj_bytes_new(PyLiteState *state, const char* str, int size, bool is_raw);
PyLiteBytesObject* pylt_obj_bytes_new_empty(PyLiteState *state);
void pylt_obj_bytes_free(PyLiteState *state, PyLiteBytesObject *self);

#endif
