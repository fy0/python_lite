
#ifndef PYLITE_TYPES_SET_H
#define PYLITE_TYPES_SET_H

#include "object.h"

KHASH_INIT(set_obj, PyLiteObject*, char, 0, pylt_obj_hash, pylt_obj_eq);

typedef struct PyLiteSetObject {
    PyLiteObject_HEAD;
    khash_t(set_obj) ob_val;
} PyLiteSetObject;


#endif
