
#ifndef PYLITE_TYPES_TYPE_H
#define PYLITE_TYPES_TYPE_H

#include "object.h"

typedef struct PyLiteTypeObject {
    PyLiteObject_HEAD;
    pl_uint32_t ob_reftype;
    pl_uint32_t ob_base;
    struct PyLiteDictObject *ob_attrs;
} PyLiteTypeObject;

PyLiteTypeObject* pylt_obj_type_new(PyLiteState *state, pl_uint32_t instance_type, pl_uint32_t base_type);

PyLiteObject* pylt_obj_type_getattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject *key);
void pylt_obj_type_setattr(PyLiteState *state, PyLiteTypeObject *self, PyLiteObject* key, PyLiteObject* value);


#endif
