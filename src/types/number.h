
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


uint32_t pylt_obj_int_hash(PyLiteState *state, PyLiteIntObject *obj);
uint32_t pylt_obj_int_eq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
uint32_t pylt_obj_int_cmp(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_plus(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);

uint32_t pylt_obj_float_hash(PyLiteState *state, PyLiteFloatObject *obj);
uint32_t pylt_obj_float_eq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
uint32_t pylt_obj_float_cmp(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_plus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteIntObject* pylt_obj_int_new(PyLiteState *state, uint32_t val);
PyLiteFloatObject* pylt_obj_float_new(PyLiteState *state, double val);

#endif
