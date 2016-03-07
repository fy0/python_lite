
#ifndef PYLITE_TYPES_BYTES_H
#define PYLITE_TYPES_BYTES_H

#include "object.h"

typedef struct PyLiteBytesObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint8_t *ob_val;
} PyLiteBytesObject;

pl_int_t pylt_obj_bytes_ccmp(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_bytes_ceq(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_bytes_chash(PyLiteState *state, PyLiteBytesObject *obj);
pl_uint32_t pylt_obj_bytes_cforcehash(PyLiteState *state, PyLiteBytesObject *obj);

PyLiteObject* pylt_obj_bytes_mul(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_bytes_plus(PyLiteState *state, PyLiteBytesObject *self, PyLiteObject *other);

PyLiteBytesObject* pylt_obj_bytes_new(PyLiteState *state, const char* str, int size, bool is_raw);
void pylt_obj_bytes_free(PyLiteState *state, PyLiteBytesObject *self);

#endif
