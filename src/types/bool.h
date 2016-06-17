
#ifndef PYLITE_TYPES_BOOL_H
#define PYLITE_TYPES_BOOL_H

#include "object.h"

typedef struct PyLiteBoolObject {
    PyLiteObject_HEAD;
    bool ob_val;
} PyLiteBoolObject;

pl_int_t pylt_obj_bool_cmp(PyLiteState *state, PyLiteBoolObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_bool_eq(PyLiteState *state, PyLiteBoolObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_bool_hash(PyLiteState *state, PyLiteBoolObject *obj);
struct PyLiteStrObject* pylt_obj_bool_to_str(PyLiteState *state, PyLiteBoolObject *self);

PyLiteBoolObject* pylt_obj_bool_new(PyLiteState *state, bool val);

PyLiteBoolObject PyLiteTrue;
PyLiteBoolObject PyLiteFalse;

#endif
