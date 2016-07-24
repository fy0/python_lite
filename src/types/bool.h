
#ifndef PYLITE_TYPES_BOOL_H
#define PYLITE_TYPES_BOOL_H

#include "object.h"

typedef struct PyLiteBoolObject {
    PyLiteObject_HEAD;
    bool ob_val;
} PyLiteBoolObject;

pl_int_t pylt_obj_bool_cmp(PyLiteInterpreter *I, PyLiteBoolObject *self, PyLiteObject *other);
pl_bool_t pylt_obj_bool_eq(PyLiteInterpreter *I, PyLiteBoolObject *self, PyLiteObject *other);
pl_uint32_t pylt_obj_bool_hash(PyLiteInterpreter *I, PyLiteBoolObject *obj);
struct PyLiteStrObject* pylt_obj_bool_to_str(PyLiteInterpreter *I, PyLiteBoolObject *self);

PyLiteBoolObject* pylt_obj_bool_new(PyLiteInterpreter *I, bool val);

PyLiteBoolObject PyLiteTrue;
PyLiteBoolObject PyLiteFalse;

#endif
