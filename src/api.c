
#include "api.h"
#include "intp.h"

void pylt_api_output_str(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    if (!obj) {
        printf("bad str\n");
        return;
    }
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


PyLiteStrObject* pylt_api_type_name(PyLiteInterpreter *I, int ob_type) {
    return kv_A(I->cls_base, ob_type)->name;
}


PyLiteTypeObject* pylt_api_gettype(PyLiteInterpreter *I, pl_uint32_t type_code) {
    return kv_A(I->cls_base, type_code);
}

pl_uint32_t pylt_api_get_base_typecode(PyLiteInterpreter *I, pl_uint32_t type_code) {
    pl_int_t top_base;

    top_base = type_code;
    while (top_base >= PYLT_OBJ_TYPE_USERCLASS) {
        top_base = pylt_api_gettype(I, top_base)->ob_base;
    }

    return top_base;
}

pl_bool_t pylt_api_isinstance(PyLiteInterpreter *I, PyLiteObject *obj, pl_uint32_t type_code) {
    PyLiteTypeObject* type;
    pl_uint32_t ob_type = obj->ob_type;

    if (type_code == PYLT_OBJ_TYPE_OBJ) {
        return true;
    }

    while (ob_type != PYLT_OBJ_TYPE_OBJ) {
        if (ob_type == type_code) return true;
        type = pylt_api_gettype(I, ob_type);
        ob_type = type->ob_base;
    }

    return false;
}

void pl_print(PyLiteInterpreter *I, const char *format, ...) {
    va_list args;
    PyLiteStrObject *str;

    va_start(args, format);
    str = pylt_obj_str_new_from_vformat(I, pylt_obj_str_new_from_cstr(I, format, true), args);
    va_end(args);
    pylt_api_output_str(I, str);
}
