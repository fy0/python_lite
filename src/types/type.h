
#ifndef PYLITE_TYPES_TYPE_H
#define PYLITE_TYPES_TYPE_H

#include "object.h"
#include "string.h"
#include "dict.h"

typedef struct PyLiteTypeObject {
    PyLiteObject_HEAD;
    PyLiteStrObject *name;
    pl_uint32_t ob_reftype;
    pl_uint32_t ob_base;
    PyLiteDictObject *ob_attrs;
} PyLiteTypeObject;

PyLiteTypeObject* pylt_obj_type_new(PyLiteState *state, PyLiteStrObject *name, pl_uint32_t instance_type, pl_uint32_t base_type);
PyLiteTypeObject* pylt_obj_type_new_with_vars(PyLiteState *state, PyLiteStrObject *name, pl_uint32_t base_type, PyLiteDictObject *dict);

PyLiteObject* pylt_obj_type_getattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject *key);
void pylt_obj_type_setattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value);
pl_uint32_t pylt_obj_type_chash(PyLiteState *state, PyLiteTypeObject *self);

#endif
