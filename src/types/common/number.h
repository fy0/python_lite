
#ifndef PYLITE_TYPES_NUMBER_H
#define PYLITE_TYPES_NUMBER_H

#include "../object.h"

typedef struct PyLiteIntObject {
    PyLiteObject_HEAD;
    pl_int_t ob_val;
} PyLiteIntObject;


typedef struct PyLiteFloatObject {
    PyLiteObject_HEAD;
    pl_float_t ob_val;
} PyLiteFloatObject;


pl_int_t pylt_obj_int_cmp(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_int_eq(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_int_hash(PyLiteInterpreter *I, PyLiteIntObject *obj);

PyLiteObject* pylt_obj_int_bitor(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitxor(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitand(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitls(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_bitrs(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_int_plus(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_minus(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_mul(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_div(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_floordiv(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_int_mod(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_int_positive(PyLiteInterpreter *I, PyLiteIntObject *self);
PyLiteObject* pylt_obj_int_negative(PyLiteInterpreter *I, PyLiteIntObject *self);
PyLiteObject* pylt_obj_int_bitnot(PyLiteInterpreter *I, PyLiteIntObject *self);
PyLiteObject* pylt_obj_int_pow(PyLiteInterpreter *I, PyLiteIntObject *self, PyLiteObject *other);


pl_int_t pylt_obj_float_cmp(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_float_eq(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_float_hash(PyLiteInterpreter *I, PyLiteFloatObject *obj);

PyLiteObject* pylt_obj_float_plus(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_minus(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_mul(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_div(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_floordiv(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);
PyLiteObject* pylt_obj_float_mod(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);

PyLiteObject* pylt_obj_float_positive(PyLiteInterpreter *I, PyLiteFloatObject *self);
PyLiteObject* pylt_obj_float_negative(PyLiteInterpreter *I, PyLiteFloatObject *self);
PyLiteObject* pylt_obj_float_pow(PyLiteInterpreter *I, PyLiteFloatObject *self, PyLiteObject *other);

uint32_t* pylt_obj_int_to_ucs4(PyLiteInterpreter *I, PyLiteIntObject *self, pl_int_t *plen);
struct PyLiteStrObject* pylt_obj_int_to_str(PyLiteInterpreter *I, PyLiteIntObject *self);
uint32_t* pylt_obj_float_to_ucs4(PyLiteInterpreter *I, PyLiteFloatObject *self, pl_int_t *plen);
struct PyLiteStrObject* pylt_obj_float_to_str(PyLiteInterpreter *I, PyLiteFloatObject *self);

PyLiteIntObject* pylt_obj_int_new(PyLiteInterpreter *I, pl_int_t val);
PyLiteFloatObject* pylt_obj_float_new(PyLiteInterpreter *I, double val);

PyLiteIntObject* pylt_obj_int_new_from_cstr_full(PyLiteInterpreter *I, const char *str, pl_int_t size, pl_int_t base_n);
PyLiteFloatObject* pylt_obj_float_new_from_cstr_full(PyLiteInterpreter *I, const char *str, pl_int_t size, pl_int_t point_pos);

void pylt_obj_int_free(PyLiteInterpreter *I, PyLiteIntObject *self);
void pylt_obj_float_free(PyLiteInterpreter *I, PyLiteFloatObject *self);

#endif
