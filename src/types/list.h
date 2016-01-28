
#ifndef PYLITE_TYPES_LIST_H
#define PYLITE_TYPES_LIST_H

#include "object.h"
#include "../lib/klist.h"

KLIST_INIT(list_obj, PyLiteObject*, pylt_obj_free)

typedef struct PyLiteTupleObject {
    PyLiteObject_HEAD;
    klist_t(list_obj) ob_val;
} PyLiteSetObject;


#endif
