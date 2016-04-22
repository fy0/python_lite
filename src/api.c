
#include "api.h"

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
