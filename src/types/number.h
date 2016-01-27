
#ifndef PYLITE_TYPES_NUMBER_H
#define PYLITE_TYPES_NUMBER_H

#include "object.h"

typedef struct PyLiteIntObject {
    PyLiteObject_HEAD;
    uint32_t ob_val;
} PyLiteIntObject;


typedef struct PyLiteFloatObject {
    PyLiteObject_HEAD;
    double ob_val;
} PyLiteFloatObject;


uint32_t pylt_obj_int_hash(PyLiteIntObject *obj);
uint32_t pylt_obj_int_eq(PyLiteObject *self, PyLiteObject *other);
uint32_t pylt_obj_int_cmp(PyLiteObject *self, PyLiteObject *other);

uint32_t pylt_obj_float_hash(PyLiteIntObject *obj);
uint32_t pylt_obj_float_eq(PyLiteObject *self, PyLiteObject *other);
uint32_t pylt_obj_float_cmp(PyLiteObject *self, PyLiteObject *other);

#endif
