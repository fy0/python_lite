
#include "api.h"
#include "intp.h"

void pylt_api_output_str(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    if (!obj) {
        printf("bad str\n");
        return;
    }
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_BYTES:
            printf("%s", castbytes(obj)->ob_val);
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


PyLiteTypeObject* pylt_api_gettype_by_code(PyLiteInterpreter *I, pl_uint32_t type_code) {
    return kv_A(I->cls_base, type_code);
}

PyLiteTypeObject* pylt_api_gettype(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *name) {
	PyLiteObject *obj = pylt_obj_mod_getattr(I, mod, castobj(name));
	return pl_istype(obj) ? casttype(obj) : NULL;
}

pl_uint32_t pylt_api_get_base_typecode(PyLiteInterpreter *I, pl_uint32_t type_code) {
    pl_int_t top_base;

    top_base = type_code;
    while (top_base >= PYLT_OBJ_TYPE_USERCLASS) {
        top_base = pylt_api_gettype_by_code(I, top_base)->ob_base;
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
        type = pylt_api_gettype_by_code(I, ob_type);
        ob_type = type->ob_base;
    }

    return false;
}

static pl_int_t _get_arg_count2(const char *format) {
    pl_int_t args_count = 0;
    if (format[0] == '\0') return 0;
    for (const char *p = format + 1; *p; ++p) {
        if (*p == '%') {
            if (*(p - 1) != '%' && *(p - 1) != '//') {
                args_count++;
            }
        }
    }
    return args_count;
}

void pl_print(PyLiteInterpreter *I, const char *format, ...) {
    va_list args;
    PyLiteStrObject *str;
    pl_int_t args_count = _get_arg_count2(format);
    PyLiteTupleObject *targs = pylt_obj_tuple_new(I, args_count);

    va_start(args, format);
    for (pl_int_t i = 0; i < args_count; ++i) {
        targs->ob_val[i] = va_arg(args, PyLiteObject*);
    }
    va_end(args);

    str = pylt_obj_str_new_from_format_with_tuple(I, pylt_obj_str_new_from_cstr(I, format, true), targs);
    pylt_api_output_str(I, str);
}
