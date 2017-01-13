
#include "vm.h"
#include "api.h"
#include "intp.h"
#include "debug.h"
#include "bind.h"
#include "types/all.h"
#include "types/objectE.h"
#include "mods/builtin.h"
#include "pybind/typebind.h"
#include "utils/misc.h"

PyLiteObject* _pylt_vm_call(PyLiteInterpreter *I, pl_int_t argc);

const wchar_t *op_vals[] = {
    L"or",
    L"and",
    L"not",
    L"in", L"not in", L"is", L"is not", L"<", L"<=", L">", L">=", L"!=", L"==",
    L"|",
    L"^",
    L"&",
    L"<<", L">>",
    L"+", L"-",
    L"*", L"@", L"/", L"//", L"%",
    L"+x", L"-x", L"~x",
    L"**",
};

const wchar_t* pylt_vm_get_op_name(int op) {
    return op_vals[op];
}

int token_to_op_val(uint32_t tk) {
    switch (tk) {
        case TK_KW_OR: return OP_OR;
        case TK_KW_AND: return OP_AND;
        case TK_KW_NOT: return OP_NOT;

        case TK_KW_IN: return OP_IN; case TK_KW_IS: return OP_IS; case '<': return OP_LT; case TK_OP_LE: return OP_LE; 
        case '>': return OP_GT; case TK_OP_GE: return OP_GE; case TK_OP_NE: return OP_NE; case TK_OP_EQ: return OP_EQ;

        case '|': return OP_BITOR; case '^': return OP_BITXOR; case '&': return OP_BITAND;
        case TK_OP_LSHIFT: return OP_LSHIFT; case TK_OP_RSHIFT: return OP_RSHIFT;
        case '+': return OP_PLUS; case '-': return OP_MINUS;
        case '*': return OP_MUL; case '@': return OP_MATMUL; case '/': return OP_DIV; case TK_OP_FLOORDIV: return OP_FLOORDIV; case '%': return OP_MOD;
        /*case '+': return OP_POS; case '-': return OP_NEG;*/  case '~': return OP_BITNOT;
        case TK_OP_POW: return OP_POW;
    }
    return 0;
}

int token_de_to_op_val(uint32_t tk) {
    switch (tk) {
        case TK_DE_PLUS_EQ: return OP_PLUS;
        case TK_DE_MINUS_EQ: return OP_MINUS;
        case TK_DE_MUL_EQ: return OP_MUL;
        case TK_DE_DIV_EQ: return OP_DIV;
        case TK_DE_FLOORDIV_EQ: return OP_FLOORDIV;
        case TK_DE_MOD_EQ: return OP_MOD;
        case TK_DE_MATMUL_EQ: return OP_MATMUL;
        case TK_DE_BITAND_EQ: return OP_BITAND;
        case TK_DE_BITOR_EQ: return OP_BITOR;
        case TK_DE_BITXOR_EQ: return OP_BITXOR;
        case TK_DE_RSHIFT_EQ: return OP_RSHIFT;
        case TK_DE_LSHIFT_EQ: return OP_LSHIFT;
        case TK_DE_POW_EQ: return OP_POW;
    }
    return 0;
}

void pylt_vm_init(struct PyLiteInterpreter *I, PyLiteVM* vm) {
    PyLiteContext *ctx;

    ctx = vm->ctx = pylt_malloc(I, sizeof(PyLiteContext));
    ctx->params_offset = 0;
    kv_init(I, ctx->stack);
    kv_init(I, ctx->frames);
    kv_init(I, ctx->expt_stack);

    // built-in
    vm->builtins = pl_getmod(I, pl_static.str.builtins);

    // first frame - global
    pylt_vm_push_code(I, NULL);
}

void pylt_vm_finalize(PyLiteInterpreter *I) {
    PyLiteContext *ctx = I->vm.ctx;
    kv_destroy(ctx->stack);
    kv_destroy(ctx->frames);
    pylt_free_ex(I, I->vm.ctx);
}

void pylt_vm_push_code(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteFrame *frame;
    PyLiteContext *ctx = I->vm.ctx;

    if (kv_size(ctx->frames)) {
        kv_top(ctx->frames).ip_saved = ctx->ip;
    }
    kv_pushp(PyLiteFrame, ctx->frames);
    frame = &kv_top(ctx->frames);
    frame->func = NULL;
    frame->code = code;
    frame->halt_when_ret = false;
    frame->locals = pylt_obj_dict_new(I);

    ctx->ip = (code) ? kv_P(frame->code->opcodes, 0) : NULL;
}

void pylt_vm_push_func(PyLiteInterpreter *I, PyLiteFunctionObject *func) {
    PyLiteFrame *frame;
    PyLiteContext *ctx = I->vm.ctx;

    if (kv_size(ctx->frames)) {
        kv_top(ctx->frames).ip_saved = ctx->ip;
    }
    kv_pushp(PyLiteFrame, ctx->frames);
    frame = &kv_top(ctx->frames);
    frame->func = func;
    frame->code = &func->code;
    frame->halt_when_ret = false;
    frame->locals = pylt_obj_dict_new(I);

    pylt_gc_add(I, castobj(func));
    //pylt_gc_add(I, castobj(func->code));
    if (func) {
        ctx->ip = (frame->code) ? kv_P(frame->code->opcodes, 0) : NULL;
    }
}


void pylt_vm_load_code(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteContext *ctx = I->vm.ctx;
    PyLiteFrame *frame = &kv_top(ctx->frames);

    frame->func = NULL;
	frame->code = code;
	frame->halt_when_ret = false;

    pylt_gc_add(I, castobj(code));
    if (code) ctx->ip = &kv_A(frame->code->opcodes, 0);
}

void pylt_vm_pop_frame(PyLiteInterpreter *I) {
    PyLiteContext *ctx = I->vm.ctx;
    kv_pop(ctx->frames);
    ctx->ip = kv_top(ctx->frames).ip_saved;
}


#define func_call_ret(val) {retval = (val); goto _err;}

// 函数调用检查
//  1 为当前的参数个数小于最少参数个数
//  2 为当前的参数个数多于参数个数
//  3 为参数的类型不符合
// -1 当前对象不是可调用对象
// -2 外部异常
//
// pflag: the kind of the callable object，such as ...
//  0 tobj is a function or cfunction object
//  1 tobj is a class, pfunc_obj is tobj.__new__ (backup parameters)
//  2 tobj is a object with __call__ attribute, pfunc_obj is tobj.__call__

int func_call_check(PyLiteInterpreter *I, PyLiteObject *tobj, int params_num, PyLiteDictObject *kwargs, PyLiteObject **pfunc_obj, PyLiteFunctionInfo **pinfo, pl_int_t *pflag, PyLiteTupleObject **params_bak) {
    PyLiteFunctionInfo *info;
    PyLiteObject *obj, *defobj;
    PyLiteObject **args;

    int args_i;
    bool at_type;
    int retval, retval_util;
    PyLiteContext *ctx = I->vm.ctx;

    info = pylt_obj_func_get_info(I, tobj);
    if (info) {
        if (pflag) *pflag = 0;
        if (pfunc_obj) *pfunc_obj = tobj;
    } else {
        if (tobj->ob_type == PYLT_OBJ_TYPE_TYPE) {
            // __new__
            obj = pylt_obj_Egetattr(I, tobj, castobj(pl_static.str.__new__), &at_type);
            if (I->error) func_call_ret(-2);

            if (obj) {
                info = pylt_obj_func_get_info(I, obj);
                if (!info) func_call_ret(-1);
				if (pflag) {
					*pflag = 1;
					// backup parameters for __init__
					if (params_num && params_bak) {
						*params_bak = pylt_obj_tuple_new_with_data(I, params_num, &kv_topn(ctx->stack, params_num-1));
					}
				}
            } else {
                // not function
                func_call_ret(-1);
            }
        } else {
            // __call__
            obj = pylt_obj_Egetattr(I, tobj, castobj(pl_static.str.__call__), &at_type);
            if (I->error) func_call_ret(-2);
            if (obj) {
                info = pylt_obj_func_get_info(I, obj);
                if (!info) func_call_ret(-1);
                if (pflag) *pflag = 2;
            } else func_call_ret(-1);
        }

        // 插入首个参数
        if (params_num == 0) kv_pushptr(ctx->stack, tobj); // __new__(cls, ...) __call__(self, ...)
        else {
            kv_pushptr(ctx->stack, NULL);
            memmove(&kv_topn(ctx->stack, params_num - 1), &kv_topn(ctx->stack, params_num), sizeof(uintptr_t) * params_num);
            kv_topn(ctx->stack, params_num) = (uintptr_t)tobj;
        }
        params_num++;

        if (pfunc_obj) *pfunc_obj = obj;
    }

    if (!info->defaults && !kwargs) {
        // 极简情况，调用时不用字典调用，函数本身也没有默认值、*args和**kwargs
        if (params_num < info->length) {
            retval_util = params_num;
            func_call_ret(1);
        }

        if (params_num > info->minimal) {
            func_call_ret(2);
        }
    } else {
        args = (PyLiteObject**)(&kv_topn(ctx->stack, params_num - 1));
        args_i = 0;

        for (int i = 0; i < info->length; ++i) {
            defobj = info->defaults ? info->defaults->ob_val[i] : castobj(&PyLiteParamUndefined);

			if (defobj == castobj(&PyLiteParamUndefined)) {
				// 如果栈中还有参数，试图填充
				// 如果没有，试图找 kwargs 填充
				// 都没有，报错，缺参数
				pl_bool_t ok = false;

				if (args_i != params_num) {
					args_i++;
					ok = true;
				} else if (kwargs) {
					obj = pylt_obj_dict_pop(I, kwargs, info->params->ob_val[i]);
					if (obj) {
						kv_pushptr(ctx->stack, obj);
						ok = true;
					}
				}

                if (!ok) {
                    retval_util = i;
					func_call_ret(1);
				}
			} else if (defobj == castobj(&PyLiteParamArgs)) {
                // 如果栈中还有参数，将剩下的参数做成 tuple 入栈
                // 如果没有，压入一个空 tuple
                if (args_i != params_num) {
                    int args_len = params_num - args_i;
                    PyLiteTupleObject *tuple = pylt_obj_tuple_new_with_data(I, args_len, args + args_i);
                    kv_popn(ctx->stack, args_len);
                    kv_pushptr(ctx->stack, tuple);
                    params_num -= args_len;
                } else {
                    PyLiteTupleObject *tuple = pylt_obj_tuple_new(I, 0);
                    kv_pushptr(ctx->stack, tuple);
                }
			} else if (defobj == castobj(&PyLiteParamKwargs)) {
				// 必定为最后一个参数，如果有 kwargs，压入
				// 如果没有 压入一个空字典
				kv_pushptr(ctx->stack, (kwargs) ? kwargs : pylt_obj_dict_new(I));
			} else {
                // 如果栈中还有参数，试图填充
                // 如果没有，试图找 kwargs 填充
                // 还没有，用预设值填充
                if (args_i != params_num) {
                    args_i++;
                    continue;
                }

                if (kwargs) {
					obj = pylt_obj_dict_pop(I, kwargs, info->params->ob_val[i]);
                    if (obj) {
                        kv_pushptr(ctx->stack, obj);
                        break;
                    }
                }

                kv_pushptr(ctx->stack, defobj);
            }
        }
        // 如果 args_i != params_num 那么参数多出来了，报错
        if (args_i != params_num) {
            func_call_ret(2);
        }
    }

    // type check
    if (info->type_codes) {
        args = (PyLiteObject**)(&kv_topn(ctx->stack, info->length - 1));

        for (int i = 0; i < info->length; ++i) {
            if (!info->type_codes[i]) continue;
            if (!pl_isinstance(I, args[i], info->type_codes[i])) {
                retval_util = i;
                func_call_ret(3);
            }
        }
    }

    if (pinfo) *pinfo = info;
    return 0;

_err:
    switch (retval) {
        case -1:
            pl_error(I, pl_static.str.TypeError, "%r object is not callable", pl_type(I, tobj));
            break;
        case -2:
            // 外部异常
            break;
        case 1: // less than minimal
            // Why TypeError?
            pl_error(I, pl_static.str.TypeError, "%s() missing %d required positional argument (%d given)", info->name, pylt_obj_int_new(I, info->minimal - params_num), pylt_obj_int_new(I, retval_util));
            break;
        case 2: // more than parameter's num
            // another version: TypeError: int() takes at most 2 arguments (3 given)
            pl_error(I, pl_static.str.TypeError, "%s() takes %d positional arguments but %d were given", info->name, pylt_obj_int_new(I, info->length), pylt_obj_int_new(I, params_num));
            break;
        case 3: // error parameter type
            pl_error(I, pl_static.str.TypeError, "%s()'s parameter %r must be %s", info->name, info->params->ob_val[retval_util], pl_type_by_code(I, info->type_codes[retval_util])->name);
            break; 
    }
    return retval;
}

#define const_obj(__index) pylt_obj_list_getitem(I, frame->code->const_val, (__index))

void pylt_vm_except_setup(PyLiteInterpreter *I, uint32_t typecode, PyLiteInstruction *ip) {
    PyLiteContext *ctx = I->vm.ctx;
    kv_pushp(PyLiteExceptInfo, ctx->expt_stack);
    PyLiteExceptInfo *ei = &kv_top(ctx->expt_stack);
    ei->frame = pylt_vm_curframe(I);
    ei->typecode = typecode;
    ei->ip_catch = ip;
}

void pylt_vm_except_pop(PyLiteInterpreter *I, int num) {
    kv_popn(I->vm.ctx->expt_stack, num);
}

pl_int_t pylt_vm_except_check(PyLiteInterpreter *I) {
    PyLiteContext *ctx = I->vm.ctx;
    PyLiteFrame *frame = &kv_top(ctx->frames);

    if (I->error) {
        PyLiteFrame *nframe = frame;
        if (kv_size(ctx->expt_stack)) {
            pl_int_t fcount = 0;
            PyLiteExceptInfo *info = &kv_top(ctx->expt_stack);
            for (pl_uint_t i = 0; i < kv_size(ctx->expt_stack); ++i) {
                if (info->frame != nframe) fcount++;
                if (pl_isinstance(I, I->error, info->typecode)) {
                    // 将下级 frame 的异常自动弹出
                    kv_popn(ctx->expt_stack, fcount);
                    // 切换帧栈
                    while (nframe != info->frame) {
                        nframe = &kv_pop(ctx->frames);
                    }
                    ctx->ip = info->ip_catch;
                    kv_pushptr(ctx->stack, I->error);
                    I->error = NULL;
                    return 2; // exception caught
                }
                info--;
            }
        }

        pl_print(I, "Traceback (most recent call last):\n");
        if (frame->code->with_debug_info) {
            pl_print(I, "  File \"<stdin>\", line %d, in <module>\n", pylt_obj_int_new(I, kv_A(frame->code->lnotab, ctx->ip - &kv_A(frame->code->opcodes, 1))));
        }
        PyLiteTupleObject *args = dcast(except, I->error)->args;
        if (args->ob_size == 0) {
            pl_print(I, "%s\n", pl_type(I, I->error)->name);
        } else {
            PyLiteObject *output = args->ob_size == 1 ? args->ob_val[0] : castobj(args);
            pl_print(I, "%s: %s\n", pl_type(I, I->error)->name, output);
        }
        return 1; // crash, shutdown
    }

    return 0;
}

PyLiteDictObject* pylt_vm_run(PyLiteInterpreter *I) {
    PyLiteVM *vm = &I->vm;
    PyLiteContext *ctx = I->vm.ctx;
    PyLiteFrame *frame = &kv_top(ctx->frames);
    PyLiteObject *tobj, *tret;

    for (;;) {
        PyLiteInstruction ins = *(ctx->ip++);
        switch (pylt_vm_except_check(I)) {
            case 1: return NULL;
            case 2: frame = &kv_top(ctx->frames); continue;
        }
        //if (...) pylt_gc_collect(I);

        switch (ins.code) {
            case BC_LOADNONE:
                // LOAD_NONE    0       0
                kv_pushptr(ctx->stack, &PyLiteNone);
                break;
            case BC_LOADCONST:
                // LOAD_CONST   0       const_id
                kv_pushptr(ctx->stack, const_obj(ins.extra));
                break;
            case BC_LOADLOCALS:
                // LOADLOCALS   0       0
                kv_pushptr(ctx->stack, frame->locals);
                break;
            case BC_SET_VAL:
                // SET_VAL      0       const_id
                pylt_obj_dict_setitem(I, frame->locals, const_obj(ins.extra), castobj(kv_top(ctx->stack)));
                break;
            case BC_SET_VALX:
                // SET_VALX     offset  const_id
                pylt_obj_dict_setitem(I, frame->locals, const_obj(ins.extra), castobj(kv_topn(ctx->stack, ins.exarg)));
                break;
            case BC_LOAD_VAL:
            case BC_LOAD_VAL_: {
                // LOAD_VAL     0       const_id
                PyLiteObject *name = const_obj(ins.extra);
                PyLiteObject *tobj = pylt_obj_dict_getitem(I, frame->locals, name); // local

                if (!tobj) { // enclose
                    if (frame->func && frame->func->info.closure) {
                        tobj = pylt_obj_dict_getitem(I, frame->func->info.closure, name);
                    }
                }

                /*PyLiteDictObject *g = kv_A(ctx->frames, 0).locals;
                pl_foreach_dict(I, it, g) {
                    PyLiteObject *key, *value;
                    pylt_obj_dict_keyvalue(I, g, it, &key, &value);
                    pl_print(I, "%s\n", key);
                    pl_print(I, "%s\n", value);
                }*/

                tobj = (tobj) ? tobj : pylt_obj_dict_getitem(I, kv_A(ctx->frames, 0).locals, name); // global
                tobj = (tobj) ? tobj : pylt_obj_mod_getattr(I, vm->builtins, caststr(name)); // builtins

                if (!tobj) {
                    pl_error(I, pl_static.str.NameError, "name %r is not defined", caststr(const_obj(ins.extra)));
                    break;
                }
                kv_push(uintptr_t, ctx->stack, (uintptr_t)tobj);
                break;
            }
            case BC_GET_ITEM:
            case BC_GET_ITEM_: {
                // GET_ITEM     0       0
                PyLiteObject *obj = castobj(kv_pop(ctx->stack));
                PyLiteObject *key = castobj(kv_pop(ctx->stack));
                tobj = pylt_obj_Egetitem(I, key, obj);
                if (!tobj) break;
                kv_push(uintptr_t, ctx->stack, (uintptr_t)tobj);
                break;
            }
            case BC_SET_ITEM: {
                // SET_ITEM     offset  0
                PyLiteObject *key = castobj(kv_pop(ctx->stack));
                PyLiteObject *tobj = castobj(kv_pop(ctx->stack));
                PyLiteObject *value = castobj(kv_topn(ctx->stack, ins.exarg));
                pylt_obj_Esetitem(I, tobj, key, value);
                if (I->error) break;
                break;
            }
            case BC_GET_ATTR:
            case BC_GET_ATTR_: {
                // GET_ATTR     0/1     const_id
                pl_bool_t at_type;
                tobj = castobj(kv_pop(ctx->stack));
                tret = pylt_obj_Egetattr_ex(I, tobj, const_obj(ins.extra), NULL, &at_type);
                if (I->error) break;
                kv_pushptr(ctx->stack, tret);

                // next instruction is BC_CALL, and it's a method!
                if (at_type) {
                    if (ins.exarg) {
                        kv_pushptr(ctx->stack, tobj);
                        ctx->params_offset = 1;
                        continue;
                    }
                    if (tret->ob_type == PYLT_OBJ_TYPE_PROP) {
                        ins.code = BC_CALL;
                        ins.exarg = 0;
                        ins.extra = 1;
                        kv_pop(ctx->stack);
                        kv_pushptr(ctx->stack, castprop(tret)->fget.func);
                        kv_pushptr(ctx->stack, tobj);
                        goto _BC_CALL;
                    }
                }
                break;
            }
            case BC_SET_ATTR: {
                // SET_ATTR     offset  const_id
                tobj = castobj(kv_pop(ctx->stack));
                PyLiteObject *value = castobj(kv_topn(ctx->stack, ins.exarg));
                if (!pylt_obj_setattr(I, tobj, const_obj(ins.extra), value)) {
                    pl_error(I, pl_static.str.TypeError, "can't set attributes of built-in/extension type %r", pl_type(I, tobj)->name);
                    break;
                }
                break;
            }
            case BC_GET_SLICE:
            case BC_GET_SLICE_: {
                // GET_SLICE    0       0
                PyLiteObject *step = castobj(kv_pop(ctx->stack));
                PyLiteObject *end = castobj(kv_pop(ctx->stack));
                PyLiteObject *start = castobj(kv_pop(ctx->stack));
                PyLiteObject *obj = castobj(kv_pop(ctx->stack));

                PyLiteObject *ret = pylt_obj_Eslice_ex(I, obj, start, end, step);
                if (I->error) break;
                kv_pushptr(ctx->stack, ret);
                break;
            }
            case BC_SET_SLICE: {
                // SET_SLICE    offset  0
                PyLiteObject *step = castobj(kv_pop(ctx->stack));
                PyLiteObject *end = castobj(kv_pop(ctx->stack));
                PyLiteObject *start = castobj(kv_pop(ctx->stack));
                PyLiteObject *obj = castobj(kv_pop(ctx->stack));
                PyLiteObject *val = castobj(kv_topn(ctx->stack, ins.extra));
                pylt_obj_Eslice_set_ex(I, obj, start, end, step, val);
                break;
            }
            case BC_OPERATOR:
                // OPERATOR     0       op
                switch (ins.extra) {
                    case OP_OR: case OP_AND: case OP_IN: case OP_NOT_IN: case OP_IS : case OP_IS_NOT:
                    case OP_LT: case OP_LE: case OP_GT: case OP_GE: case OP_NE: case OP_EQ:
                    case OP_BITOR: case OP_BITXOR: case OP_BITAND: case OP_LSHIFT: case OP_RSHIFT:
                    case OP_PLUS: case OP_MINUS: case OP_MUL: case OP_MATMUL: case OP_DIV: case OP_FLOORDIV: case OP_MOD: case OP_POW: {
                        PyLiteObject *objb = castobj(kv_pop(ctx->stack));
                        PyLiteObject *obja = castobj(kv_pop(ctx->stack));
                        tret = pylt_obj_op_binary(I, ins.extra, obja, objb);
                        if (!tret) {
                            pl_error(I, pl_static.str.TypeError, "unsupported operand type(s) for %s: %r and %r",
                                pl_strnew_w(I, pylt_vm_get_op_name(ins.extra), true),
                                pl_type(I, obja)->name,
                                pl_type(I, objb)->name
                            );
                            break;
                        }
                        pylt_gc_add(I, tret);
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tret);
                        break;
                    }
                    default: {
                        PyLiteObject *obj = castobj(kv_pop(ctx->stack));
                        tret = pylt_obj_op_unary(I, ins.extra, obj);
                        if (!tret) {
                            pl_error(I, pl_static.str.TypeError, "bad operand type for unary %s: %r",
                                pl_strnew_w(I, pylt_vm_get_op_name(ins.extra), true),
                                pl_type(I, obj)->name
                            );
                            break;
                        }
                        pylt_gc_add(I, tret);
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tret);
                        break;
                    }
                }
                break;
            case BC_NEW_OBJ:
                // NEW_OBJ      type    extra
                switch (ins.exarg) {
                    case PYLT_OBJ_TYPE_ITER:
                        tret = castobj(pylt_obj_iter_new(I, castobj(kv_pop(ctx->stack))));
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tret);
                        break;
                    case PYLT_OBJ_TYPE_FUNCTION: {
                        PyLiteObject *kwargs_name = castobj(kv_pop(ctx->stack));
                        PyLiteObject *args_name = castobj(kv_pop(ctx->stack));
                        PyLiteObject *params = castobj(kv_pop(ctx->stack));
                        PyLiteObject *tcode = castobj(kv_pop(ctx->stack));
                        PyLiteObject *name = castobj(kv_pop(ctx->stack));
                        PyLiteObject *defaults = castobj(kv_pop(ctx->stack));

                        PyLiteFunctionObject *tfunc = pylt_obj_func_new_ex(
                            I,
                            caststr(name), // name
                            castlist(params), // params
                            (PyLiteCodeObject*)tcode, // code
                            castdict((defaults != castobj(&PyLiteNone)) ? defaults : NULL), // defaults
                            caststr((args_name != castobj(&PyLiteNone)) ? args_name : NULL), // args_name
                            caststr((kwargs_name != castobj(&PyLiteNone)) ? kwargs_name : NULL) // kwargs_name
                        );

                        pylt_gc_add(I, castobj(tfunc));
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tfunc);
                        break;
                    }
                    case PYLT_OBJ_TYPE_SET:
                        tobj = castobj(pylt_obj_set_new(I));
                        for (pl_int_t j = 0; j < ins.extra; ++j) {
                            pylt_obj_set_add(I, castset(tobj), castobj(kv_pop(ctx->stack)));
                        }
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_LIST:
                        tobj = castobj(pylt_obj_list_new_with_size(I, ins.extra));
                        for (pl_uint_t j = ins.extra; j > 0; --j) {
                            pylt_obj_list_append(I, castlist(tobj), castobj(kv_A(ctx->stack, kv_size(ctx->stack) - j)));
                        }
                        kv_popn(ctx->stack, ins.extra);
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_TUPLE:
                        tobj = castobj(pylt_obj_tuple_new(I, ins.extra));
                        memcpy(casttuple(tobj)->ob_val, &kv_topn(ctx->stack, ins.extra - 1), sizeof(PyLiteObject*) * ins.extra);
                        kv_popn(ctx->stack, ins.extra);
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_DICT:
                        tobj = castobj(pylt_obj_dict_new(I));
                        for (pl_uint_t j = ins.extra; j > 0; j--) {
                            pylt_obj_dict_setitem(I, castdict(tobj), castobj(kv_topn(ctx->stack, j * 2 - 1)), castobj(kv_topn(ctx->stack, (j - 1) * 2)));
                        }
                        kv_popn(ctx->stack, ins.extra * 2);
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, ctx->stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_TYPE: {
                        PyLiteObject *name = castobj(kv_pop(ctx->stack)); // name
                        PyLiteObject *basetype = castobj(kv_pop(ctx->stack)); // base type
                        PyLiteObject *cls_vars = castobj(kv_pop(ctx->stack)); // class vars

                        basetype = pl_isnone(basetype) ? castobj(pl_type_by_code(I, PYLT_OBJ_TYPE_OBJ)) : basetype;
                        PyLiteTypeObject *type = pylt_obj_type_new(I, caststr(name), casttype(basetype)->ob_reftype, castdict(cls_vars));
                        // TOOD: current mod
                        pylt_type_register(I, NULL, type);
                        pylt_gc_add(I, castobj(type));
                        kv_pushptr(ctx->stack, type);
                        break;
                    }
                }
                break;
			case BC_CALL:
                _BC_CALL: {
                // BC_CALL      0       params_num
                pl_int_t tflag;
                pl_uint_t params_num = ins.extra + ctx->params_offset;
                ctx->params_offset = 0;

                PyLiteDictObject *kwargs = ins.exarg ? castdict(kv_pop(ctx->stack)) : NULL; // kwargs
                tobj = castobj(kv_topn(ctx->stack, params_num)); // 函数对象

                // check
                PyLiteFunctionInfo *func_info;
				PyLiteTupleObject *params_bak = NULL;
				if (func_call_check(I, tobj, params_num, kwargs, &tret, &func_info, &tflag, &params_bak)) {
                    break;
                }

                // set frame->locals and execute
                I->recent_called = tret;
                if (tret->ob_type == PYLT_OBJ_TYPE_FUNCTION) {
                    pylt_vm_push_func(I, castfunc(tret));
                    frame = &kv_top(ctx->frames);

					if (func_info->length > 0) {
						for (int k = 0; k < func_info->length; ++k) {
							pylt_obj_dict_setitem(I, frame->locals, castobj(func_info->params->ob_val[k]), castobj(kv_topn(ctx->stack, func_info->length - k - 1)));
						}
					}
					kv_popn(ctx->stack, func_info->length + 1); // pop args and func obj

					if (tflag != 1) {
						// tobj is func or has a __call__ method
                        //ctx->ip = &kv_A(frame->code->opcodes, 0);
					} else {
						// tobj is a type, new object
						frame->halt_when_ret = true;
                        pylt_vm_run(I);
                        frame = &kv_top(ctx->frames);
					}
                } else if (tret->ob_type == PYLT_OBJ_TYPE_CFUNCTION) {
                    tret = castcfunc(tret)->code(I, func_info->length, (PyLiteObject**)(&kv_topn(ctx->stack, func_info->length - 1)));
					kv_popn(ctx->stack, func_info->length + 1);
                    if (I->error) break;
					if (tret) kv_pushptr(ctx->stack, tret);
					else kv_pushptr(ctx->stack, (uintptr_t)pylt_obj_none_new(I));
                }

                // if new instance created
                if (tflag == 1) {
                    // call __init__
					tret = castobj(kv_top(ctx->stack));

					if ((!pl_istype(tret)) && tret->ob_type == casttype(tobj)->ob_reftype) {
						PyLiteObject *method_func = pylt_obj_Egetattr(I, tret, castobj(pl_static.str.__init__), NULL);
                        if (I->error) break;
						if (method_func) {
                            pl_call_method_ex(I, tret, method_func, params_bak, kwargs);
						}
						if (params_bak) pylt_obj_tuple_free(I, params_bak);
					}
                }
                break;
			}
            case BC_RET:
                // RET          0       with_value
                if (!ins.extra) kv_pushptr(ctx->stack, &PyLiteNone);
				if (frame->halt_when_ret) {
					kv_pop(ctx->frames);
					goto _end;
				}

                pylt_vm_pop_frame(I);
                frame = &kv_top(ctx->frames);
                break;
            case BC_TEST:
                // TEST         0       jump_offset
                if (!pylt_obj_istrue(I, castobj(kv_pop(ctx->stack)))) {
                    ctx->ip += ins.extra;
                }
                break;
            case BC_JMP:
                // JMP          0       offset
                ctx->ip += ins.extra;
                break;
            case BC_JMP_BACK:
                // JMP_BACK     0       offset
                ctx->ip -= ins.extra;
                break;
            case BC_FORITER: {
                // FORITER      0       jump_offset
                tret = pylt_obj_iter_next(I, castiter(kv_top(ctx->stack)));
                if (tret == NULL) ctx->ip += ins.extra;
                else kv_push(uintptr_t, ctx->stack, (uintptr_t)tret);
                break;
            }
            case BC_DEL_NAME:
                // DEL_VAL      0       const_id
                if (!pylt_obj_dict_remove(I, frame->locals, const_obj(ins.extra))) {
                    pl_error(I, pl_static.str.UnboundLocalError, "local variable %r referenced before assignment", const_obj(ins.extra));
                }
                break;
            case BC_DEL_ATTR: {
                // DEL_ATTR     0       const_id
                tobj = castobj(kv_pop(ctx->stack));
                pylt_obj_Edelattr(I, tobj, const_obj(ins.extra));
                break;
            }
            case BC_DEL_ITEM: {
                // DEL_ITEM     0       0
                PyLiteObject *key = castobj(kv_pop(ctx->stack));
                tobj = castobj(kv_pop(ctx->stack));
                pylt_obj_Edelitem(I, tobj, key);
                break;
            }
            case BC_DEL_SLICE: {
                // SET_SLICE    offset  0
                PyLiteObject *step = castobj(kv_pop(ctx->stack));
                PyLiteObject *end = castobj(kv_pop(ctx->stack));
                PyLiteObject *start = castobj(kv_pop(ctx->stack));
                PyLiteObject *obj = castobj(kv_pop(ctx->stack));
                PyLiteObject *val = castobj(kv_topn(ctx->stack, ins.extra));
                pylt_obj_Eslice_del_ex(I, obj, start, end, step, val);
                break;
            }
            case BC_DEL_FORCE:
                // DEL_FORCE    0       0
                //pylt_free(I, castobj(kv_pop(ctx->stack)));
                break;
            case BC_POP:
                // POP          0       0
                kv_pop(ctx->stack);
                break;
            case BC_POPN:
                // POPN         0       num
                kv_popn(ctx->stack, ins.extra);
                break;
            case BC_ASSERT: {
                // ASSERT       0       extra
                if (ins.extra) tret = castobj(kv_pop(ctx->stack));
                if (!pylt_obj_istrue(I, castobj(kv_pop(ctx->stack)))) {
                    if (ins.extra) pl_error(I, pl_static.str.AssertionError, "%s", tret);
                    else pl_error(I, pl_static.str.AssertionError, NULL);
                }
                break;
            }
            case BC_IMPORT_NAME: {
                // IMPORT_NAME  0       N
                if (ins.extra > 1) {
                    pl_error(I, pl_static.str.NotImplementedError, NULL);
                    break;
                }
                PyLiteObject *name = castobj(kv_topn(ctx->stack, ins.extra - 1));
                PyLiteModuleObject *mod = pl_getmod(I, caststr(name));
                if (mod) {
                    pylt_obj_dict_setitem(I, frame->locals, name, castobj(mod));
                    kv_popn(ctx->stack, ins.extra);
                } else {
                    PyLiteFile *input = pylt_io_file_new(I, pl_cformat(I, "%s.py", name), pl_cformat(I, "r"), PYLT_IOTE_UTF8);
                    if (!input) break;
                    PyLiteCodeObject *tcode = pylt_intp_parsef(I, input);
#ifdef PL_DEBUG_INFO
                    pl_print(I, "======== module load: %s ========\n", name);
                    debug_print_const_vals(I, tcode);
                    debug_print_opcodes(I, tcode);
                    pl_print(I, "======== module end: %s ========\n", name);
#endif
                    kv_popn(ctx->stack, ins.extra);
                    pylt_vm_push_code(I, tcode);
                    PyLiteDictObject *scope = pylt_vm_run(I);
                    pylt_vm_pop_frame(I);
                    frame = &kv_top(ctx->frames);
                    if (I->error) goto _end;
                    PyLiteModuleObject *mod = pylt_obj_module_new(I, NULL, caststr(name));
                    mod->ob_attrs = scope;
                    pylt_obj_dict_setitem(I, frame->locals, name, castobj(mod));
                }
                break;
            }
            case BC_UNPACK_SEQ: {
                // UNPACK_SEQ   0       0
                tret = castobj(kv_pop(ctx->stack));
                PyLiteIterObject *iter = pylt_obj_iter_Enew(I, tret);
                if (I->error) break;
                for (PyLiteObject *obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
                    kv_pushptr(ctx->stack, obj);
                }
                break;
            }
            case BC_UNPACK_ARG: {
                // UNPACK_ARG   0       0
                pl_int_t count = 0;
                tret = castobj(kv_pop(ctx->stack));
                PyLiteIterObject *iter = pylt_obj_iter_Enew(I, tret);
                if (I->error) break;
                for (PyLiteObject *obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
                    kv_pushptr(ctx->stack, obj);
                    count++;
                }
                ctx->params_offset += count;
                break;
            }
            case BC_DICT_COMBINE: {
                // DICT_COMBINE 0       0
                // b = pop()
                // a = pop()
                // a.update(b)
                PyLiteDictObject *b = castdict(kv_pop(ctx->stack));
                PyLiteDictObject *a = castdict(kv_pop(ctx->stack)); // 必然为 dict（自动生成）
                if (!pl_isdict(b)) {
                    pl_error(I, pl_static.str.TypeError, "function argument after ** must be a mapping, not %s", pl_type(I, castobj(a))->name);
                    break;
                }
                pylt_obj_dict_update(I, a, b);
                kv_pushptr(ctx->stack, a);
                break;
            }
            case BC_EXPT_SETUP: {
                // SETUP_EXCEPT 0       jump_offset
                PyLiteTypeObject *et = casttype(kv_pop(ctx->stack));
                pylt_vm_except_setup(I, et->ob_reftype, ctx->ip + ins.extra);
                break;
            }
            case BC_EXPT_POPN: {
                // POPN_EXCEPT  0       num
                pylt_vm_except_pop(I, ins.extra);
                break;
            }
            case BC_RAISE: {
                // RAISE        0       0/1
                if (ins.extra) {
                    PyLiteObject *e = castobj(kv_pop(ctx->stack));
                    // e is exception type
                    if (pl_istype(e)) {
                        if (pl_issubclass(I, casttype(e), pl_type_by_code(I, PYLT_OBJ_TYPE_BASE_EXCEPTION))) {
                            pl_error_by_type(I, casttype(e), NULL);
                            break;
                        }
                        pl_error(I, pl_static.str.RuntimeError, "exceptions must derive from BaseException");
                        break;
                    }
                    if (!pl_isinstance(I, e, PYLT_OBJ_TYPE_BASE_EXCEPTION)) {
                        pl_error(I, pl_static.str.RuntimeError, "exceptions must derive from BaseException");
                        break;
                    }
                    pl_raise(I, e);
                    break;
                }
                pl_error(I, pl_static.str.RuntimeError, "No active exception to reraise");
                break;
            }
            case BC_NOP:
                break;
            case BC_PRINT:
                if (kv_size(ctx->stack) != 0) {
                    debug_print_obj(I, castobj(kv_top(ctx->stack)));
                    putwchar('\n');
                }
                pl_print(I, "locals: %s\n", castobj(frame->locals));
                break;
            case BC_HALT:
                goto _end;
        }
    }
_end:
    return frame->locals;
}

PyLiteDictObject *pl_vm_get_locals(PyLiteInterpreter *I) {
    PyLiteFrame *frame = &kv_top(I->vm.ctx->frames);
    return frame->locals;
}

PyLiteFrame* pylt_vm_curframe(PyLiteInterpreter *I) {
    return &kv_top(I->vm.ctx->frames);
}
