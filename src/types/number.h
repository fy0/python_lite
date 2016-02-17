
#ifndef PYLITE_TYPES_NUMBER_H
#define PYLITE_TYPES_NUMBER_H

#include "object.h"

typedef struct PyLiteIntObject {
    PyLiteObject_HEAD;
    int32_t ob_val;
} PyLiteIntObject;


typedef struct PyLiteFloatObject {
    PyLiteObject_HEAD;
    double ob_val;
} PyLiteFloatObject;


uint32_t pylt_obj_int_chash(PyLiteState *state, PyLiteIntObject *obj);
uint32_t pylt_obj_int_ceq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
int32_t pylt_obj_int_ccmp(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_int_bitor(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitxor(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitand(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitls(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitrs(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_int_lt(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_le(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_gt(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_ge(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_ne(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_eq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_int_eq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_plus(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_minus(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_mul(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_div(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_floordiv(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_mod(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_int_positive(PyLiteState *state, PyLiteIntObject *self);
PyLiteObject* pylt_obj_int_negative(PyLiteState *state, PyLiteIntObject *self);
PyLiteObject* pylt_obj_int_bitnot(PyLiteState *state, PyLiteIntObject *self);
PyLiteObject* pylt_obj_int_pow(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);


uint32_t pylt_obj_float_chash(PyLiteState *state, PyLiteFloatObject *obj);
uint32_t pylt_obj_float_ceq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
int32_t pylt_obj_float_ccmp(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_float_lt(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_le(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_gt(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_ge(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_ne(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_eq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_float_eq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_plus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_minus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_mul(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_div(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_floordiv(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_mod(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_float_positive(PyLiteState *state, PyLiteFloatObject *self);
PyLiteObject* pylt_obj_float_negative(PyLiteState *state, PyLiteFloatObject *self);
PyLiteObject* pylt_obj_float_pow(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteIntObject* pylt_obj_int_new(PyLiteState *state, int32_t val);
PyLiteFloatObject* pylt_obj_float_new(PyLiteState *state, double val);

#endif
