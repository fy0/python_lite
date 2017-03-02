
#include "api.h"
#include "intp.h"
#include "utils/debug.h"
#include "types/all.h"
#include "utils/misc.h"

PyLiteModuleObject* pl_getmod(PyLiteInterpreter *I, PyLiteStrObject *modpath) {
    PyLiteModuleObject *mod = castmod(pylt_obj_dict_getitem(I, I->modules, castobj(modpath)));
    if (!mod) {
        PyLiteCPtrObject *cptr = castcptr(pylt_obj_dict_getitem(I, I->cmodules_loader, castobj(modpath)));
        if (cptr) {
            PyLiteModuleLoaderFunc loader = (PyLiteModuleLoaderFunc)cptr->ob_ptr;
            mod = (*loader)(I);
            mod->name = modpath;
            pylt_obj_dict_setitem(I, I->modules, castobj(modpath), castobj(mod));
        }
    }
    return mod;
}

PyLiteTypeObject* pl_modtype(PyLiteInterpreter *I, PyLiteModuleObject *mod, PyLiteStrObject *name) {
    PyLiteObject *obj = pylt_obj_mod_getattr(I, mod, name);
    if (!obj) return NULL;
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

void pl_outputstr(PyLiteInterpreter *I, PyLiteStrObject *obj) {
    if (!obj) {
        wprintf(L"bad str\n");
        return;
    }
#ifndef PYLT_EXTMOD
    if (sizeof(wchar_t) == sizeof(uint32_t)) {
        wprintf((const wchar_t*)obj->ob_val);
    } else {
        for (pl_uint_t i = 0; i < obj->ob_size; ++i) {
            putwchar((wchar_t)obj->ob_val[i]);
        }
    }
#else
    pylt_io_file_writestr(I, I->sys.cout, obj->ob_val, obj->ob_size, ' ');
#endif
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
    PyLiteTypeObject *type = casttype(pylt_obj_mod_getattr(I, I->vm.builtins, exception_name));
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


void pl_error_by_type(PyLiteInterpreter *I, PyLiteTypeObject *exception_type, const char *format, ...) {
    PyLiteObject *error;
    PyLiteStrObject *str;
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
        error = pl_call(I, castobj(exception_type), 1, str);
        I->error = error;
    } else {
        error = pl_call(I, castobj(exception_type), 0);
    }
    I->error = error;
}

void pl_raise(PyLiteInterpreter *I, PyLiteObject *exception) {
    I->error = exception;
}

#define kv_pushins(v, x) kv_push(PyLiteInstruction, (v), (x))

PyLiteObject* _pl_call(PyLiteInterpreter *I, pl_int_t argc) {
    PyLiteInstruction bc_call = { .code = BC_CALL, .exarg = 0, .extra = argc };
    PyLiteInstruction bc_halt = { .code = BC_HALT, .exarg = 0, .extra = 0 };
    PyLiteCodeObject *code = pylt_obj_code_new(I, false);

    kv_pushins(code->opcodes, bc_call);
    kv_pushins(code->opcodes, bc_halt);
    pylt_vm_push_code(I, code);
    pylt_vm_run(I);
    pylt_vm_pop_frame(I);

    return castobj(kv_pop(I->vm.ctx->stack));
}

PyLiteObject* pl_call(PyLiteInterpreter *I, PyLiteObject *callable, int argc, ...) {
    va_list args;
    PyLiteContext *ctx = I->vm.ctx;
    kv_pushptr(ctx->stack, callable);

    va_start(args, argc);
    for (pl_int_t i = 0; i < argc; ++i) {
        kv_pushptr(ctx->stack, va_arg(args, PyLiteObject*));
    }
    va_end(args);

    return _pl_call(I, argc);
}

PyLiteObject* pl_call_method(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *callable, int argc, ...) {
    va_list args;
    PyLiteContext *ctx = I->vm.ctx;

    kv_pushptr(ctx->stack, callable);
    kv_pushptr(ctx->stack, self);

    va_start(args, argc);
    for (pl_int_t i = 0; i < argc; ++i) {
        kv_pushptr(ctx->stack, va_arg(args, PyLiteObject*));
    }
    va_end(args);

    return _pl_call(I, argc + 1);
}

PyLiteObject* pl_call_method_ex(PyLiteInterpreter *I, PyLiteObject *self, PyLiteObject *callable, PyLiteTupleObject *args, PyLiteDictObject *kwargs) {
    pl_int_t argc = (args) ? args->ob_size : 0;
    PyLiteContext *ctx = I->vm.ctx;
    kv_pushptr(ctx->stack, callable);
    kv_pushptr(ctx->stack, self);

    if (args) {
        for (pl_int_t i = 0; i < argc; ++i) {
            kv_pushptr(ctx->stack, args->ob_val[i]);
        }
    }
    if (kwargs) {
        kv_pushptr(ctx->stack, kwargs);
        argc++;
    }

    return _pl_call(I, argc + 1);
}


PyLiteModuleObject* load_module(PyLiteInterpreter *I, PyLiteStrObject *fn) {
    PyLiteFile *input = pylt_io_openRead(I, fn);
    if (!input) return NULL;
    PyLiteCodeObject *tcode = pylt_intp_parsef(I, input);
#ifdef PL_DEBUG_INFO
    pl_print(I, "======== module load: %s ========\n", name);
    debug_print_const_vals(I, tcode);
    debug_print_opcodes(I, tcode);
    pl_print(I, "======== module end: %s ========\n", name);
#endif
    pylt_vm_push_code(I, tcode);
    PyLiteDictObject *scope = pylt_vm_run(I);
    pylt_vm_pop_frame(I);

    PyLiteFrame *frame = pylt_vm_curframe(I);
    if (I->error) return NULL;
    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL);
    mod->ob_attrs = scope;
    return mod;
}

// 注意：
// 1. 此处 names 永远是绝对路径，也就是相对根路径来描述的
// 2. names 是包路径的分解，例如 'a.b.c' -> ['a', 'b', 'c']
// Python 的奇怪规则：
// a(目录，带__init__.py) > a.py > a(目录)
PyLiteModuleObject* pylt_api_import(PyLiteInterpreter *I, PyLiteStrObject **names, pl_int_t names_num) {
    pl_int_t nindex = 0;
    PyLiteStrObject *fn, *name = NULL;
    PyLiteStrObject *nextname;
    PyLiteModuleObject *mod;
    if (names_num == 0) return NULL;

    // 最简情况
    if (names_num == 1) {
        mod = pl_getmod(I, names[0]);
        if (mod) return mod;

        fn = pl_cformat(I, "%s.py", names[0]);
        pl_bool_t ret = I->sys.deffs->exists(I, fn);
        if (ret) {
            mod = load_module(I, fn);
            if (I->error) return NULL;
            mod->name = names[0];
            return mod;
        }

        if (I->sys.deffs->isdir(I, names[0])) {
            PyLiteStrObject *fn2 = pl_cformat(I, "%s/__init__.py", names[0]);
            if (I->sys.deffs->exists(I, fn2)) {
                mod = load_module(I, fn2);
                if (I->error) return NULL;
                mod->name = names[0];
                return mod;
            } else {
                return pylt_obj_module_new(I, names[0]);
            }
        }

        if (!ret) {
            pl_error(I, pl_static.str.ImportError, "No module named %r", names[0]);
            return NULL;
        }
    }

    do {
        nextname = names[nindex];
        if (name) {
            name = caststr(pylt_obj_str_plus(I, name, castobj(pl_strnew_w(I, L".", true))));
            name = caststr(pylt_obj_str_plus(I, name, castobj(nextname)));
        } else name = nextname;

        mod = pl_getmod(I, caststr(name));
        if (!mod) {
            PyLiteFile *input = pylt_io_openRead(I, pl_cformat(I, "%s.py", name));
            if (!input) return NULL;
            PyLiteCodeObject *tcode = pylt_intp_parsef(I, input);
#ifdef PL_DEBUG_INFO
            pl_print(I, "======== module load: %s ========\n", name);
            debug_print_const_vals(I, tcode);
            debug_print_opcodes(I, tcode);
            pl_print(I, "======== module end: %s ========\n", name);
#endif
            pylt_vm_push_code(I, tcode);
            PyLiteDictObject *scope = pylt_vm_run(I);
            pylt_vm_pop_frame(I);

            PyLiteFrame *frame = pylt_vm_curframe(I);
            if (I->error) return NULL;
            PyLiteModuleObject *mod = pylt_obj_module_new(I, caststr(name));
            mod->ob_attrs = scope;
        }

        pylt_obj_dict_setitem(I, I->modules, castobj(name), castobj(mod));
    } while (((++nindex) == names_num));
    return NULL;
}
