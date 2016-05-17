
#ifndef PYLITE_TYPES_EXCEPTION_H
#define PYLITE_TYPES_EXCEPTION_H

#include "../object.h"
#include "../string.h"
#include "../tuple.h"

typedef struct PyLiteExceptionObject {
    PyLiteObject_HEAD;
    PyLiteTupleObject *args;
} PyLiteExceptionObject;

PyLiteExceptionObject* pylt_obj_exception_new(PyLiteState *state);

#endif
