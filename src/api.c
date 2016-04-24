
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



const char* pylt_obj_basetypes[] = {
    NULL, // 0
    "object",
    "int",
    "float",
    "bool",

    "str",
    "bytes",
    "set",
    "list",
    "tuple",
    "dict",

    "module",
    "function",
    "cfunction",
    "codesnippet",

    "type",
    "iter",
    "class",
    "none",
};

const char* pylt_obj_type_name_cstr(PyLiteState *state, PyLiteObject *obj) {
    if (obj->ob_type < PYLT_OBJ_TYPE_CLASS) {
        return pylt_obj_basetypes[obj->ob_type];
    }
    return NULL;
}

const char* pylt_type_name(int ob_type) {
    return pylt_obj_basetypes[ob_type];
}
