
#include "api.h"
#include "state.h"

void pylt_obj_output_str(PyLiteState *state, PyLiteStrObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            printf("%s", castbytes(obj)->ob_size, castbytes(obj)->ob_val);
            break;
        case PYLT_OBJ_TYPE_STR:
            for (size_t i = 0; i < caststr(obj)->ob_size; i++) {
                putcode(caststr(obj)->ob_val[i]);
            }
            break;
    }
}

PyLiteTypeObject* pylt_gettype(PyLiteState *state, pl_uint32_t type_code) {
    return kv_A(state->cls_base, type_code);
}
