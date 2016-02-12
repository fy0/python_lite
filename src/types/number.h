
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
uint32_t pylt_obj_int_eq(PyLiteIntObject *self, PyLiteObject *other);
uint32_t pylt_obj_int_cmp(PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_plus(PyLiteIntObject *self, PyLiteObject *other);

uint32_t pylt_obj_float_hash(PyLiteFloatObject *obj);
uint32_t pylt_obj_float_eq(PyLiteFloatObject *self, PyLiteObject *other);
uint32_t pylt_obj_float_cmp(PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_plus(PyLiteFloatObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_number_from_stream(StringStream *ss);

PyLiteIntObject* pylt_obj_int_new(uint32_t val);
PyLiteFloatObject* pylt_obj_float_new(double val);

#endif
