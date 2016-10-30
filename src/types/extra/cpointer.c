
#include "cpointer.h"

pl_uint32_t pylt_obj_cptr_hash(PyLiteInterpreter *I, PyLiteCPtrObject *obj) {
    return (pl_uint32_t)obj->ob_ptr;
}

PyLiteCPtrObject* pylt_obj_cptr_new(PyLiteInterpreter *I, void *ptr) {
    PyLiteCPtrObject *obj = pylt_malloc(I, sizeof(PyLiteCPtrObject));
    obj->ob_type = PYLT_OBJ_TYPE_CPTR;
    obj->ob_flags = 0;
    obj->ob_ptr = ptr;
    return obj;
}
