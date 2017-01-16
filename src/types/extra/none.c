
#include "none.h"

PyLiteNoneObject PyLiteNone = { PYLT_OBJ_TYPE_NONE, PYLT_OBJ_FLAG_STATIC };


pl_int_t pylt_obj_none_cmp(PyLiteInterpreter *I, PyLiteNoneObject *self, PyLiteObject *other) {
    return 2;
}

pl_bool_t pylt_obj_none_eq(PyLiteInterpreter *I, PyLiteNoneObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_NONE:
            return true;
        default:
            return false;
    }
}

pl_uint32_t pylt_obj_none_chash(PyLiteInterpreter *I, PyLiteNoneObject *obj) {
    return 0xCAFEBABE;
}

PyLiteNoneObject* pylt_obj_none_new(PyLiteInterpreter *I) {
    return &PyLiteNone;
}
