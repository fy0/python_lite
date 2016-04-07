
#include "none.h"

PyLiteNoneObject PyLiteNone = { PYLT_OBJ_TYPE_NONE };


pl_int_t pylt_obj_none_ccmp(PyLiteState *state, PyLiteNoneObject *self, PyLiteObject *other) {
    return 2;
}

pl_bool_t pylt_obj_none_ceq(PyLiteState *state, PyLiteNoneObject *self, PyLiteObject *other) {
    switch (other->ob_type) {
        case PYLT_OBJ_TYPE_NONE:
            return true;
        default:
            return false;
    }
}

pl_uint32_t pylt_obj_none_chash(PyLiteState *state, PyLiteNoneObject *obj) {
    return 0xCAFEBABE;
}

PyLiteNoneObject* pylt_obj_none_new(PyLiteState *state) {
    return &PyLiteNone;
}
