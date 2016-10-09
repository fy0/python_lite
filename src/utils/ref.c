
#include "ref.h"

PyLiteRef* pl_ref_new(PyLiteInterpreter *I, PyLiteObject *obj) {
    PyLiteRef *ref = pylt_malloc(I, sizeof(PyLiteRef));
    ref->obj = obj;
    return ref;
}


void pl_ref_free(PyLiteInterpreter *I, PyLiteRef *ref) {
    pylt_free_ex(I, ref);
}
