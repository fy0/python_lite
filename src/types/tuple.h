
#ifndef PYLITE_TYPES_TUPLE_H
#define PYLITE_TYPES_TUPLE_H

#include "object.h"

typedef struct PyLiteTupleObject {
    PyLiteObject_HEAD;
    int ob_size;
    PyLiteObject *ob_val;
} PyLiteTupleObject;


#endif
