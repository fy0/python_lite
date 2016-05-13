
#ifndef PYLITE_TYPES_NUMBER_H
#define PYLITE_TYPES_NUMBER_H

#include "object.h"

typedef struct PyLiteIntObject {
    PyLiteObject_HEAD;
    pl_int_t ob_val;
} PyLiteIntObject;


typedef struct PyLiteFloatObject {
    PyLiteObject_HEAD;
    double ob_val;
} PyLiteFloatObject;


pl_int_t pylt_obj_int_cmp(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_int_eq(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_int_hash(PyLiteState *state, PyLiteIntObject *obj);

PyLiteObject* pylt_obj_int_bitor(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitxor(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitand(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitls(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitrs(PyLiteState *state, PyLiteIntObject *self, PyLiteObject *other);

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


pl_int_t pylt_obj_float_cmp(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_float_eq(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_float_hash(PyLiteState *state, PyLiteFloatObject *obj);

PyLiteObject* pylt_obj_float_plus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_minus(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_mul(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_div(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_floordiv(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_mod(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_float_positive(PyLiteState *state, PyLiteFloatObject *self);
PyLiteObject* pylt_obj_float_negative(PyLiteState *state, PyLiteFloatObject *self);
PyLiteObject* pylt_obj_float_pow(PyLiteState *state, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteIntObject* pylt_obj_int_new(PyLiteState *state, pl_int_t val);
PyLiteFloatObject* pylt_obj_float_new(PyLiteState *state, double val);

PyLiteIntObject* pylt_obj_int_new_from_cstr_full(PyLiteState *state, const char *str, pl_int_t size, pl_int_t base_n);
PyLiteFloatObject* pylt_obj_float_new_from_cstr_full(PyLiteState *state, const char *str, pl_int_t size, pl_int_t point_pos);

#endif
