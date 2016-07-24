
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
    pl_bool_t allow_inherit;
} PyLiteTypeObject;

PyLiteTypeObject* pylt_obj_type_new_with_type(PyLiteInterpreter *I, PyLiteStrObject *name, pl_uint32_t instance_type, pl_uint32_t base_type);
PyLiteTypeObject* pylt_obj_type_new(PyLiteInterpreter *I, PyLiteStrObject *name, pl_uint32_t base_type, PyLiteDictObject *dict);

PyLiteObject* pylt_obj_type_getattr(PyLiteInterpreter *I, PyLiteTypeObject *self, PyLiteObject *key, pl_bool_t *p_at_type);
void pylt_obj_type_setattr(PyLiteInterpreter *I, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value);
pl_uint32_t pylt_obj_type_hash(PyLiteInterpreter *I, PyLiteTypeObject *self);
void pylt_obj_type_free(PyLiteInterpreter *I, PyLiteTypeObject *self);

#endif
