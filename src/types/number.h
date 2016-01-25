
#ifndef PYLITE_TYPES_NUMBER_H
#define PYLITE_TYPES_NUMBER_H

#include "object.h"

typedef struct PyLiteIntObject {
    PyLiteObject_HEAD
    uint32_t ob_val;
} PyLiteIntObject;

bool pylt_obj_is_int(PyLiteObject *obj);
uint32_t pylt_obj_int_hash(PyLiteIntObject *obj);
uint32_t pylt_obj_int_cmp(PyLiteObject *a, PyLiteObject *b);

#endif
