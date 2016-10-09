
#ifndef PYLITE_TYPES_CUTSOM_H
#define PYLITE_TYPES_CUTSOM_H

#include "object.h"
#include "dict.h"

// Custom object

typedef struct PyLiteCustomObject {
    uint32_t ob_type;
    uint32_t ob_flags;
    uint32_t ob_base;
    PyLiteDictObject *ob_attrs;
    PyLiteObject *base_obj;
} PyLiteCustomObject;

#define PyLiteCustomObject_HEAD \
    uint32_t ob_type; \
    uint32_t ob_flags; \
    uint32_t ob_base; \
    PyLiteDictObject *ob_attrs

PyLiteObject* pylt_obj_cutstom_create(PyLiteInterpreter *I, uint32_t ob_type, PyLiteObject *base_obj);
PyLiteObject* pylt_obj_custom_getattr(PyLiteInterpreter *I, PyLiteCustomObject *self, PyLiteObject *key, pl_bool_t *p_at_type);
void pylt_obj_custom_setattr(PyLiteInterpreter *I, PyLiteCustomObject *self, PyLiteObject* key, PyLiteObject* value);

#endif
