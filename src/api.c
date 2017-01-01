
#include "api.h"
#include "intp.h"
#include "types/all.h"
#include "utils/misc.h"

void pl_outputstr(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    if (!obj) {
        wprintf(L"bad str\n");
        return;
    }
    pl_io_file_writestr(I, I->sys.cout, obj->ob_val, obj->ob_size, ' ');
}

PyLiteTypeObject* pl_getbuiltintype(PyLiteInterpreter *I, PyLiteStrObject *name) {
    return pl_gettype_from_mod(I, I->vm.builtins, name);
}

PyLiteTypeObject* pl_gettype_from_mod(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *name) {
	PyLiteObject *obj = pylt_obj_mod_getattr(I, mod, castobj(name));
	return pl_istype(obj) ? casttype(obj) : NULL;
}

pl_uint32_t pl_getbase_by_code(PyLiteInterpreter *I, pl_uint32_t type_code) {
    pl_int_t top_base;

    top_base = type_code;
    while (top_base >= PYLT_OBJ_TYPE_USERCLASS) {
        top_base = pl_type_by_code(I, top_base)->ob_base;
    }

    return top_base;
}

pl_bool_t pl_isinstance(PyLiteInterpreter *I, PyLiteObject *obj, pl_uint32_t type_code) {
    PyLiteTypeObject* type;
    pl_uint32_t ob_type = obj->ob_type;

    if (type_code == PYLT_OBJ_TYPE_OBJ) {
        return true;
    }

    while (ob_type != PYLT_OBJ_TYPE_OBJ) {
        if (ob_type == type_code) return true;
        type = pl_type_by_code(I, ob_type);
        ob_type = type->ob_base;
    }

    return false;
}

static pl_int_t _get_arg_count_cstr(const char *format) {
    pl_int_t args_count = 0;
    if (format[0] == '\0') return 0;
    if (format[0] == '%') args_count++;
    for (const char *p = format + 1; *p; ++p) {
        if (*p == '%') {
            char last = *(p - 1);
            if (last == '%') args_count--;
            else if (last != '//') {
                args_count++;
            }
        }
    }
    return args_count;
}

pl_bool_t pl_issubclass(PyLiteInterpreter *I, PyLiteTypeObject *a, PyLiteTypeObject *b) {
    if (a == b) return true;
    else {
        PyLiteTypeObject* type = a;
        while (type->ob_reftype != PYLT_OBJ_TYPE_OBJ) {
            type = pl_type_by_code(I, type->ob_base);
            if (type == b) return true;
        }
    }
    return false;
}

PyLiteTypeObject* pl_type(PyLiteInterpreter *I, PyLiteObject *obj) {
    return kv_A(I->cls_base, obj->ob_type);
}

PyLiteTypeObject* pl_type_by_code(PyLiteInterpreter *I, pl_uint32_t type_code) {
    return kv_A(I->cls_base, type_code);
}

void pl_print(PyLiteInterpreter *I, const char *format, ...) {
    va_list args;
    PyLiteStrObject *str;
    pl_int_t args_count = _get_arg_count_cstr(format);
    PyLiteTupleObject *targs = pylt_obj_tuple_new(I, args_count);

    va_start(args, format);
    for (pl_int_t i = 0; i < args_count; ++i) {
        targs->ob_val[i] = va_arg(args, PyLiteObject*);
    }
    va_end(args);

    str = pl_formatT(I, pylt_obj_str_new_from_cstr(I, format, true), targs);
    pylt_obj_tuple_free(I, targs);
    pl_outputstr(I, str);
}

void pl_error(PyLiteInterpreter *I, PyLiteStrObject *exception_name, const char *format, ...) {
    PyLiteObject *error;
    PyLiteStrObject *str;
    PyLiteTypeObject *type = pl_getbuiltintype(I, exception_name);
    va_list args;

    if (format) {
        // build error string
        pl_int_t args_count = _get_arg_count_cstr(format);
        PyLiteTupleObject *targs = pylt_obj_tuple_new(I, args_count);

        va_start(args, format);
        for (pl_int_t i = 0; i < args_count; ++i) {
            targs->ob_val[i] = va_arg(args, PyLiteObject*);
        }
        va_end(args);

        str = pl_formatT(I, pylt_obj_str_new_from_cstr(I, format, true), targs);
        pylt_obj_tuple_free(I, targs);

        // new Exception
        error = pl_call(I, castobj(type), 1, str);
        I->error = error;
    } else {
        error = pl_call(I, castobj(type), 0);
    }
    I->error = error;
}

#define kv_pushins(v, x) kv_push(PyLiteInstruction, (v), (x))

PyLiteObject* _pl_call(PyLiteInterpreter *I, pl_int_t argc) {
    PyLiteInstruction bc_call = { .code = BC_CALL, .exarg = 0, .extra = argc };
    PyLiteInstruction bc_halt = { .code = BC_HALT, .exarg = 0, .extra = 0 };
    PyLiteCodeObject *code = pylt_obj_code_new(I, false);
    PyLiteFrame frame_bak = kv_top(I->vm.frames);

    kv_pushins(code->opcodes, bc_call);
    kv_pushins(code->opcodes, bc_halt);
    pylt_vm_run(I, code);

    kv_top(I->vm.frames) = frame_bak;
    return castobj(kv_pop(I->vm.stack));
}

PyLiteObject* pl_call(PyLiteInterpreter *I, PyLiteObject *callable, int argc, ...) {
    va_list args;
    kv_pushptr(I->vm.stack, callable);

    va_start(args, argc);
    for (pl_int_t i = 0; i < argc; ++i) {
        kv_pushptr(I->vm.stack, va_arg(args, PyLiteObject*));
    }
    va_end(args);

    return _pl_call(I, argc);
}

PyLiteObject* pl_call_method(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *callable, int argc, ...) {
    va_list args;

    kv_pushptr(I->vm.stack, callable);
    kv_pushptr(I->vm.stack, self);

    va_start(args, argc);
    for (pl_int_t i = 0; i < argc; ++i) {
        kv_pushptr(I->vm.stack, va_arg(args, PyLiteObject*));
    }
    va_end(args);

    return _pl_call(I, argc + 1);
}

PyLiteObject* pl_call_method_ex(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *callable, PyLiteTupleObject *args, PyLiteDictObject *kwargs) {
    pl_int_t argc = (args) ? args->ob_size : 0;
    kv_pushptr(I->vm.stack, callable);
    kv_pushptr(I->vm.stack, self);

    if (args) {
        for (pl_int_t i = 0; i < argc; ++i) {
            kv_pushptr(I->vm.stack, args->ob_val[i]);
        }
    }
    if (kwargs) {
        kv_pushptr(I->vm.stack, kwargs);
        argc++;
    }

    return _pl_call(I, argc + 1);
}
