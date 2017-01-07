
#include "ref.h"
#include "../gc.h"

PyLiteRef* pylt_ref_new(PyLiteInterpreter *I, PyLiteObject *obj) {
    PyLiteRef *ref = pylt_malloc(I, sizeof(PyLiteRef));
    ref->obj = obj;
    pylt_gc_ref_add(I, ref);
    return ref;
}


void pylt_ref_free(PyLiteInterpreter *I, PyLiteRef *ref) {
    pylt_gc_ref_remove(I, ref);
    pylt_free_ex(I, ref);
}
