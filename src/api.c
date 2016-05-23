
#include "api.h"
#include "state.h"

void pylt_api_output_str(PyLiteState *state, PyLiteStrObject *obj) {
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
    "code",

    "type",
    "iter",
    "property",
    "none",
};

PyLiteStrObject* pylt_api_type_name(PyLiteState *state, int ob_type) {
    return kv_A(state->cls_base, ob_type)->name;
}

const char* pylt_api_type_name_cstr(PyLiteState *state, int ob_type) {
    if (ob_type <= PYLT_OBJ_BUILTIN_TYPE_NUM) {
        return pylt_obj_basetypes[ob_type];
    } else {
        return "custom class";
    }
}

PyLiteTypeObject* pylt_api_gettype(PyLiteState *state, pl_uint32_t type_code) {
    return kv_A(state->cls_base, type_code);
}

pl_uint32_t pylt_api_get_base_typecode(PyLiteState *state, pl_uint32_t type_code) {
    pl_int_t top_base;

    top_base = type_code;
    while (top_base >= PYLT_OBJ_TYPE_USERCLASS) {
        top_base = pylt_api_gettype(state, top_base)->ob_base;
    }

    return top_base;
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
