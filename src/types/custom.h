
#ifndef PYLITE_TYPES_CUTSOM_H
#define PYLITE_TYPES_CUTSOM_H

#include "object.h"
#include "dict.h"

// Custom object

typedef struct PyLiteCustomObject {
    uint32_t ob_type;
    uint32_t ob_base;
    PyLiteDictObject *ob_attrs;
} PyLiteCustomObject;

#define PyLiteCustomObject_HEAD \
    uint32_t ob_type; \
    uint32_t ob_base; \
    PyLiteDictObject *ob_attrs

PyLiteObject* pylt_obj_cutstom_new(PyLiteState *state, uint32_t ob_type);

#endif
