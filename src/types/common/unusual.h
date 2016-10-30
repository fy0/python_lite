
#ifndef PYLITE_TYPES_UNUSUAL_H
#define PYLITE_TYPES_UNUSUAL_H

#include "../object.h"

typedef struct PyLiteUnusualObject {
    PyLiteObject_HEAD;
} PyLiteUnusualObject;


// unique values
PyLiteUnusualObject PyLiteUnknown;
PyLiteUnusualObject PyLiteUseless;

// to fill defaults of function info
PyLiteUnusualObject PyLiteParamArgs;
PyLiteUnusualObject PyLiteParamKwargs;
PyLiteUnusualObject PyLiteParamUndefined;

struct PyLiteStrObject* pylt_obj_unusual_to_str(PyLiteInterpreter *I, PyLiteUnusualObject *self);

#endif
