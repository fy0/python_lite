
#ifndef PYLITE_TYPES_CPTR_H
#define PYLITE_TYPES_CPTR_H

#include "../object.h"

typedef struct PyLiteCPtrObject {
    PyLiteObject_HEAD;
    void *ob_ptr;
} PyLiteCPtrObject;

pl_uint32_t pylt_obj_cptr_hash(PyLiteInterpreter *I, PyLiteCPtrObject *obj);

PyLiteCPtrObject* pylt_obj_cptr_new(PyLiteInterpreter *I, void *ptr);

#endif
