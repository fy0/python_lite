
#include "cpointer.h"
#include "../../gc.h"

pl_uint32_t pylt_obj_cptr_hash(PyLiteInterpreter *I, PyLiteCPtrObject *obj) {
    return (pl_uint32_t)obj->ob_ptr;
}

PyLiteCPtrObject* pylt_obj_cptr_new(PyLiteInterpreter *I, void *ptr, pl_bool_t is_free_ptr) {
    PyLiteObject_init(I, obj, PyLiteCPtrObject, PYLT_OBJ_TYPE_CPTR);
    obj->ob_ptr = ptr;
    obj->auto_free = is_free_ptr;
    return obj;
}
