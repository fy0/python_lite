
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

const char* pylt_api_type_name(int ob_type) {
    return pylt_obj_basetypes[ob_type];
}

PyLiteTypeObject* pylt_api_gettype(PyLiteState *state, pl_uint32_t type_code) {
    return kv_A(state->cls_base, type_code);
}


pl_bool_t pylt_api_isinstance(PyLiteState *state, PyLiteObject *obj, pl_uint32_t type_code) {
    PyLiteTypeObject* type;
    pl_uint32_t ob_type = obj->ob_type;

    if (type_code == PYLT_OBJ_TYPE_OBJ) {
        return true;
    }

    while (ob_type != PYLT_OBJ_TYPE_OBJ) {
        if (ob_type == type_code) return true;
        type = pylt_api_gettype(state, ob_type);
        ob_type = type->ob_base;
    }

    return false;
}
