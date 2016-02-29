
#ifndef PYLITE_TYPES_DICT_H
#define PYLITE_TYPES_DICT_H

#include "object.h"

typedef struct PyLiteDictObject {
    PyLiteObject_HEAD;
    PyLiteTable *ob_val;
} PyLiteDictObject;

PyLiteDictObject* pylt_obj_dict_new(PyLiteState *state);

#endif
