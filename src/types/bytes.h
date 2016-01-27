
#ifndef PYLITE_TYPES_BYTES_H
#define PYLITE_TYPES_BYTES_H

#include "object.h"

typedef struct PyLiteBytesObject {
    PyLiteObject_HEAD;
    uint32_t ob_hash;
    uint32_t ob_size;
    uint8_t *ob_val;
} PyLiteBytesObject;

uint32_t pylt_obj_bytes_hash(PyLiteBytesObject *obj);
uint32_t pylt_obj_bytes_eq(PyLiteBytesObject *self, PyLiteObject *other);
uint32_t pylt_obj_bytes_cmp(PyLiteBytesObject *self, PyLiteObject *other);

#endif
