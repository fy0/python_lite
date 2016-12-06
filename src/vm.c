
#include "vm.h"
#include "api.h"
#include "intp.h"
#include "debug.h"
#include "bind.h"
#include "deps/kvec.h"
#include "types/all.h"
#include "mods/builtin.h"
#include "pybind/typebind.h"
#include "utils/with_exceptions.h"

PyLiteObject* _pylt_vm_call(PyLiteInterpreter *I, pl_int_t argc);

const char* op_vals[] = {
    "or",
    "and",
    "not",
    "in", "not in", "is", "is not", "<", "<=", ">", ">=", "!=", "==",
    "|",
    "^",
    "&",
    "<<", ">>",
    "+", "-",
    "*", "@", "/", "//", "%",
    "+x", "-x", "~x",
    "**",
};

const char* pylt_vm_get_op_name(int op) {
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
        case TK_OP_LSHIFT: OP_LSHIFT; case TK_OP_RSHIFT: return OP_RSHIFT;
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
    PyLiteFrame *frame;

    kv_init(I, vm->stack);
    kv_init(I, vm->frames);

    // first frame
    kv_pushp(PyLiteFrame, vm->frames);
    frame = &kv_A(vm->frames, 0);
    frame->func = NULL;
	frame->halt_when_ret = false;
    kv_init(I, frame->var_tables);

    // built-in
    pylt_bind_all_types_register(I);

    vm->builtins = pylt_mods_builtins_register(I);
    kv_push(PyLiteDictObject*, frame->var_tables, vm->builtins->attrs);

    // first local
    kv_push(PyLiteDictObject*, frame->var_tables, pylt_obj_dict_new(I));
}

void pylt_vm_finalize(PyLiteInterpreter *I) {
    kv_destroy(I->vm.stack);
    kv_destroy(I->vm.frames);
}

void pylt_vm_load_func(PyLiteInterpreter *I, PyLiteFunctionObject *func) {
    PyLiteFrame *frame;
    PyLiteVM *vm = &I->vm;
    int index = kv_size(vm->frames);

    kv_pushp(PyLiteFrame, vm->frames);
    frame = &kv_A(vm->frames, index);
    frame->func = func;
	frame->code = &func->code;
	frame->halt_when_ret = false;
    kv_init(I, frame->var_tables);

    pylt_gc_add(I, castobj(func));
    //pylt_gc_add(I, castobj(func->code));

    if (index) {
        kv_copy1(PyLiteDictObject*, frame->var_tables, kv_A(vm->frames, index - 1).var_tables);
    }

    kv_push(PyLiteDictObject*, frame->var_tables, pylt_obj_dict_new(I));
}


void pylt_vm_load_code(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteVM *vm = &I->vm;
    PyLiteFrame *frame = &kv_top(vm->frames);
    frame->func = NULL;
	frame->code = code;
	frame->halt_when_ret = false;
    pylt_gc_add(I, castobj(code));
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
    PyLiteObject *insert_caller = NULL;
    PyLiteObject **args;

    int args_i;
    bool at_type;
    int retval, retval_util;

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
						*params_bak = pylt_obj_tuple_new_with_data(I, params_num, &kv_topn(I->vm.stack, params_num-1));
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
        if (params_num == 0) kv_pushptr(I->vm.stack, tobj); // __new__(cls, ...) __call__(self, ...)
        else {
            kv_pushptr(I->vm.stack, NULL);
            memcpy(&kv_topn(I->vm.stack, params_num-1), &kv_topn(I->vm.stack, params_num), sizeof(uintptr_t) * params_num);
            kv_topn(I->vm.stack, params_num) = (uintptr_t)tobj;
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
        args = (PyLiteObject**)(&kv_topn(I->vm.stack, params_num - 1));
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
						kv_pushptr(I->vm.stack, obj);
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
                    kv_popn(I->vm.stack, args_len);
                    kv_pushptr(I->vm.stack, tuple);
                    params_num -= args_len;
                } else {
                    PyLiteTupleObject *tuple = pylt_obj_tuple_new(I, 0);
                    kv_pushptr(I->vm.stack, tuple);
                }
			} else if (defobj == castobj(&PyLiteParamKwargs)) {
				// 必定为最后一个参数，如果有 kwargs，压入
				// 如果没有 压入一个空字典
				kv_pushptr(I->vm.stack, (kwargs) ? kwargs : pylt_obj_dict_new(I));
			} else {
                // 如果栈中还有参数，试图填充
                // 如果没有，试图找 kwargs 填充
                // 还没有，用预设值填充
                if (args_i != params_num) {
                    args_i++;
                    break;
                }

                if (kwargs) {
					obj = pylt_obj_dict_pop(I, kwargs, info->params->ob_val[i]);
                    if (obj) {
                        kv_pushptr(I->vm.stack, obj);
                        break;
                    }
                }

                kv_pushptr(I->vm.stack, defobj);
            }
        }
        // 如果 args_i != params_num 那么参数多出来了，报错
        if (args_i != params_num) {
            func_call_ret(2);
        }
    }

    // type check
    if (info->type_codes) {
        args = (PyLiteObject**)(&kv_topn(I->vm.stack, info->length - 1));

        for (int i = 0; i < info->length; ++i) {
            if (!info->type_codes[i]) continue;
            if (!pylt_api_isinstance(I, args[i], info->type_codes[i])) {
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
            pl_error(I, pl_static.str.TypeError, "%r object is not callable", pylt_api_type_name(I, tobj->ob_type));
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
            pl_error(I, pl_static.str.TypeError, "%s()'s parameter %r must be %s", info->name, info->params->ob_val[retval_util], pylt_api_type_name(I, info->type_codes[retval_util]));
            break; 
    }
    return retval;
}

#define const_obj(__index) pylt_obj_list_getitem(I, code->const_val, (__index))

void pylt_vm_run(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteVM *vm = &I->vm;
    PyLiteDictObject *locals;
    PyLiteInstruction ins;

    pl_int_t tflag;
    pl_bool_t at_type;
    PyLiteFunctionObject *tfunc;
    PyLiteObject *tobj, *tret, *ta, *tb, *tc, *td, *te, *tf;

    pl_uint_t params_num;
    pl_uint_t params_offset = 0;

    if (code) pylt_vm_load_code(I, code);
    else if (!kv_top(vm->frames).code) return;
    code = kv_top(vm->frames).code;

    locals = kv_top(kv_top(vm->frames).var_tables);

    for (pl_uint_t i = 0; ; ++i) {
        // raise error
        if (I->error) {
            // TOOD: except
            pl_print(I, "Traceback (most recent call last):\n");
            if (code->with_debug_info) {
                pl_print(I, "  File \"<stdin>\", line %d, in <module>\n", pylt_obj_int_new(I, kv_A(code->lnotab, i-1)));
            }
            PyLiteTupleObject *args = dcast(except, I->error)->args;
            if (args->ob_size == 0) {
                pl_print(I, "%s\n", pylt_api_type_name(I, I->error->ob_type));
            } else {
                PyLiteObject *output = args->ob_size == 1 ? args->ob_val[0] : castobj(args);
                pl_print(I, "%s: %s\n", pl_type(I, I->error)->name, output);
            }
            return;
        }
        ins = kv_A(code->opcodes, i);
        //if (i && (i % 25 == 0)) pylt_gc_collect(I);

        switch (ins.code) {
            case BC_LOADNONE:
                // LOAD_NONE    0       0
                kv_pushptr(vm->stack, &PyLiteNone);
                break;
            case BC_LOADCONST:
                // LOAD_CONST   0       const_id
                kv_pushptr(vm->stack, const_obj(ins.extra));
                break;
            case BC_LOADLOCALS:
                // LOADLOCALS   0       0
                kv_pushptr(vm->stack, locals);
                break;
            case BC_SET_VAL:
                // SET_VAL      0       const_id
                pylt_obj_dict_setitem(I, locals, const_obj(ins.extra), castobj(kv_top(vm->stack)));
                break;
            case BC_SET_VALX:
                // SET_VALX     offset  const_id
                pylt_obj_dict_setitem(I, locals, const_obj(ins.extra), castobj(kv_topn(vm->stack, ins.exarg)));
                break;
            case BC_LOAD_VAL:
            case BC_LOAD_VAL_:
                // LOAD_VAL     0       const_id
                tobj = pylt_obj_dict_getitem(I, locals, const_obj(ins.extra));

                if (!tobj) {
                    for (int j = kv_size(kv_top(vm->frames).var_tables) - 2; j >= 0; --j) {
                        tobj = pylt_obj_dict_getitem(I, kv_A(kv_top(vm->frames).var_tables, j), const_obj(ins.extra));
                        if (tobj) break;
                    }
                }

                if (!tobj) {
                    pl_error(I, pl_static.str.NameError, "name %r is not defined", caststr(const_obj(ins.extra)));
                    break;
                }
                kv_push(uintptr_t, I->vm.stack, (uintptr_t)tobj);
                break;
            case BC_GET_ITEM:
            case BC_GET_ITEM_:
                // GET_ITEM     0       0
                tb = castobj(kv_pop(I->vm.stack));
                ta = castobj(kv_pop(I->vm.stack));
                tobj = pylt_obj_Egetitem(I, ta, tb);
                if (!tobj) break;
                kv_push(uintptr_t, I->vm.stack, (uintptr_t)tobj);
                break;
            case BC_SET_ITEM:
                // SET_ITEM     offset  0
                ta = castobj(kv_pop(vm->stack));
                tobj = castobj(kv_pop(vm->stack));
                tb = castobj(kv_topn(vm->stack, ins.exarg));
                pylt_obj_Esetitem(I, tobj, ta, tb);
                if (I->error) break;
                break;
            case BC_GET_ATTR:
            case BC_GET_ATTR_:
                // GET_ATTR     0/1     const_id
                tobj = castobj(kv_pop(I->vm.stack));
                tret = pylt_obj_Egetattr_ex(I, tobj, const_obj(ins.extra), NULL, &at_type);
                if (I->error) break;
                kv_pushptr(vm->stack, tret);

                // next instruction is BC_CALL, and it's a method!
                if (!pl_istype(tobj) && at_type) {
                    if (ins.exarg) {
                        kv_pushptr(vm->stack, tobj);
                        params_offset = 1;
                        continue;
                    }
                    if (tret->ob_type == PYLT_OBJ_TYPE_PROP) {
                        ins.code = BC_CALL;
                        ins.exarg = 0;
                        ins.extra = 1;
                        kv_pop(vm->stack);
                        kv_pushptr(vm->stack, castprop(tret)->fget.func);
                        kv_pushptr(vm->stack, tobj);
                        goto _BC_CALL;
                    }
                }
                break;
            case BC_SET_ATTR:
                // SET_ATTR     offset  const_id
                tobj = castobj(kv_pop(vm->stack));
                tb = castobj(kv_topn(vm->stack, ins.exarg));
                if (!pylt_obj_setattr(I, tobj, const_obj(ins.extra), tb)) {
                    pl_error(I, pl_static.str.TypeError, "can't set attributes of built-in/extension type %r", pylt_api_type_name(I, tobj->ob_type));
                    break;
                }
                break;
            case BC_GET_SLICE:
            case BC_GET_SLICE_: {
                // GET_SLICE    0       0
                PyLiteObject *step = castobj(kv_pop(vm->stack));
                PyLiteObject *end = castobj(kv_pop(vm->stack));
                PyLiteObject *start = castobj(kv_pop(vm->stack));
                PyLiteObject *obj = castobj(kv_pop(vm->stack));

                PyLiteObject *ret = pylt_obj_Eslice_ex(I, obj, start, end, step);
                if (I->error) break;
                kv_pushptr(vm->stack, ret);
                break;
            }
            case BC_SET_SLICE: {
                // SET_SLICE    offset  0
                PyLiteIntObject *step = castint(kv_pop(vm->stack));
                PyLiteIntObject *end = castint(kv_pop(vm->stack));
                PyLiteIntObject *start = castint(kv_pop(vm->stack));
                PyLiteObject *obj = castobj(kv_pop(vm->stack));
                PyLiteObject *val = castobj(kv_topn(vm->stack, ins.extra));

                pylt_obj_Eslice_set_ex(I, obj, start, end, step, val);
                break;
            }
            case BC_OPERATOR:
                // OPERATOR     0       op
                switch (ins.extra) {
                    case OP_OR: case OP_AND: case OP_IN: case OP_IS: case OP_IS_NOT:
                    case OP_LT: case OP_LE: case OP_GT: case OP_GE: case OP_NE: case OP_EQ:
                    case OP_BITOR: case OP_BITXOR: case OP_BITAND: case OP_LSHIFT: case OP_RSHIFT:
                    case OP_PLUS: case OP_MINUS: case OP_MUL: case OP_MATMUL: case OP_DIV: case OP_FLOORDIV: case OP_MOD: case OP_POW:
                        tb = castobj(kv_pop(I->vm.stack));
                        ta = castobj(kv_pop(I->vm.stack));
                        tret = pylt_obj_op_binary(I, ins.extra, ta, tb);
                        if (!tret) {
                            pl_error(I, pl_static.str.TypeError, "unsupported operand type(s) for %s: %r and %r", 
                                pylt_obj_str_new_from_cstr(I, pylt_vm_get_op_name(ins.extra), true),
                                pylt_api_type_name(I, ta->ob_type),
                                pylt_api_type_name(I, tb->ob_type)
                            );
                            break;
                        }
                        pylt_gc_add(I, tret);
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tret);
                        break;
                    default:
                        ta = castobj(kv_pop(I->vm.stack));
                        tret = pylt_obj_op_unary(I, ins.extra, ta);
                        if (!tret) {
                            pl_error(I, pl_static.str.TypeError, "bad operand type for unary %s: %r",
                                pylt_obj_str_new_from_cstr(I, pylt_vm_get_op_name(ins.extra), true),
                                pylt_api_type_name(I, ta->ob_type)
                            );
                            break;
                        }
                        pylt_gc_add(I, tret);
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tret);
                        break;
                }
                break;
            case BC_NEW_OBJ:
                // NEW_OBJ      type    extra
                switch (ins.exarg) {
                    case PYLT_OBJ_TYPE_ITER:
                        tret = castobj(pylt_obj_iter_new(I, castobj(kv_pop(I->vm.stack))));
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tret);
                        break;
                    case PYLT_OBJ_TYPE_FUNCTION:
                        te = castobj(kv_pop(I->vm.stack)); // kwargs_name
                        td = castobj(kv_pop(I->vm.stack)); // args_name
                        tc = castobj(kv_pop(I->vm.stack)); // params
                        tb = castobj(kv_pop(I->vm.stack)); // code
                        ta = castobj(kv_pop(I->vm.stack)); // name
                        tf = castobj(kv_pop(I->vm.stack)); // defaults

                        tfunc = pylt_obj_func_new_ex(
                            I,
                            caststr(ta), // name
                            castlist(tc), // params
                            (PyLiteCodeObject*)tb, // code
                            castdict((tf != castobj(&PyLiteNone)) ? tf : NULL), // defaults
                            caststr((td != castobj(&PyLiteNone)) ? td : NULL), // args_name
                            caststr((te != castobj(&PyLiteNone)) ? te : NULL) // kwargs_name
                        );

                        pylt_gc_add(I, castobj(tfunc));
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tfunc);
                        break;
                    case PYLT_OBJ_TYPE_SET:
                        tobj = castobj(pylt_obj_set_new(I));
                        for (pl_int_t j = 0; j < ins.extra; ++j) {
                            pylt_obj_set_add(I, castset(tobj), castobj(kv_pop(I->vm.stack)));
                        }
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_LIST:
                        tobj = castobj(pylt_obj_list_new_with_size(I, ins.extra));
                        for (pl_uint_t j = ins.extra; j > 0; --j) {
                            pylt_obj_list_append(I, castlist(tobj), castobj(kv_A(I->vm.stack, kv_size(I->vm.stack) - j)));
                        }
                        kv_popn(I->vm.stack, ins.extra);
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_TUPLE:
                        tobj = castobj(pylt_obj_tuple_new(I, ins.extra));
                        memcpy(casttuple(tobj)->ob_val, &kv_topn(vm->stack, ins.extra - 1), sizeof(PyLiteObject*) * ins.extra);
                        kv_popn(I->vm.stack, ins.extra);
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, I->vm.stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_DICT:
                        tobj = castobj(pylt_obj_dict_new(I));
                        for (pl_uint_t j = ins.extra; j > 0; j--) {
                            pylt_obj_dict_setitem(I, castdict(tobj), castobj(kv_topn(vm->stack, j * 2 - 1)), castobj(kv_topn(vm->stack, (j - 1) * 2)));
                        }
                        kv_popn(vm->stack, ins.extra * 2);
                        pylt_gc_add(I, tobj);
                        kv_push(uintptr_t, vm->stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_TYPE: {
                        ta = castobj(kv_pop(vm->stack)); // name
                        tb = castobj(kv_pop(vm->stack)); // base type
                        tc = castobj(kv_pop(vm->stack)); // class vars

                        tb = (tb->ob_type == PYLT_OBJ_TYPE_NONE) ? castobj(pylt_api_gettype_by_code(I, PYLT_OBJ_TYPE_OBJ)) : tb;
                        PyLiteTypeObject *type = pylt_obj_type_new(I, caststr(ta), casttype(tb)->ob_reftype, castdict(tc));
                        pylt_obj_type_register(I, type);
                        pylt_gc_add(I, castobj(type));
                        kv_pushptr(vm->stack, type);
                        break;
                    }
                }
                break;
			case BC_CALL: {
                _BC_CALL:
                // BC_CALL      0       params_num
                params_num = ins.extra + params_offset;
                params_offset = 0;

                PyLiteDictObject *kwargs = ins.exarg ? castdict(kv_pop(vm->stack)) : NULL; // kwargs
                tobj = castobj(kv_topn(vm->stack, params_num)); // 函数对象

                // check
                PyLiteFunctionInfo *func_info;
				PyLiteTupleObject *params_bak = NULL;
				if (func_call_check(I, tobj, params_num, kwargs, &tret, &func_info, &tflag, &params_bak)) {
                    break;
                }

                // set locals and execute
                if (tret->ob_type == PYLT_OBJ_TYPE_FUNCTION) {
					kv_top(vm->frames).code_pointer_slot = i;
					pylt_vm_load_func(I, castfunc(tret));
					locals = kv_top(kv_top(vm->frames).var_tables);
					if (func_info->length > 0) {
						for (int k = 0; k < func_info->length; ++k) {
							pylt_obj_dict_setitem(I, locals, castobj(func_info->params->ob_val[k]), castobj(kv_topn(vm->stack, func_info->length - k - 1)));
						}
					}
					kv_popn(vm->stack, func_info->length + 1); // pop args and func obj

					if (tflag != 1) {
						// tobj is func or has a __call__ method
						code = kv_top(vm->frames).code;
						i = -1;
					} else {
						// tobj is a type, new object
						kv_top(vm->frames).halt_when_ret = true;
						pylt_vm_run(I, NULL);
						locals = kv_top(kv_top(vm->frames).var_tables);
					}
                } else if (tret->ob_type == PYLT_OBJ_TYPE_CFUNCTION) {
                    tret = castcfunc(tret)->code(I, func_info->length, (PyLiteObject**)(&kv_topn(vm->stack, func_info->length - 1)));
					kv_popn(vm->stack, func_info->length + 1);
					if (tret) kv_pushptr(I->vm.stack, tret);
					else kv_pushptr(I->vm.stack, (uintptr_t)pylt_obj_none_new(I));
                }

                // if new instance created
                if (tflag == 1) {
                    // call __init__
					tret = castobj(kv_top(vm->stack));

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
                if (!ins.extra) kv_pushptr(I->vm.stack, &PyLiteNone);
				if (kv_top(vm->frames).halt_when_ret) {
					kv_pop(vm->frames);
					goto _end;
				}

                kv_pop(vm->frames);
                code = kv_top(vm->frames).code;
                locals = kv_top(kv_top(vm->frames).var_tables);
                i = kv_top(vm->frames).code_pointer_slot;
                break;
            case BC_TEST:
                // TEST         0       jump_offset
                if (!pylt_obj_istrue(I, castobj(kv_pop(I->vm.stack)))) {
                    i += ins.extra;
                }
                break;
            case BC_JMP:
                // JMP          0       offset
                i += ins.extra;
                break;
            case BC_JMP_BACK:
                // JMP_BACK     0       offset
                i -= ins.extra;
                break;
            case BC_FORITER:
                // FORITER      0       jump_offset
                ta = pylt_obj_iter_next(I, castiter(kv_top(I->vm.stack)));
                if (ta == NULL) i += ins.extra - 1;
                else kv_push(uintptr_t, I->vm.stack, (uintptr_t)ta);
                break;
            case BC_DEL_NAME:
                // DEL_VAL      0       const_id
                if (!pylt_obj_dict_remove(I, locals, const_obj(ins.extra))) {
                    pl_error(I, pl_static.str.UnboundLocalError, "local variable %r referenced before assignment", const_obj(ins.extra));
                }
                break;
            case BC_DEL_ATTR: {
                // DEL_ATTR     0       const_id
                tobj = castobj(kv_pop(vm->stack));
                pylt_obj_Edelattr(I, tobj, const_obj(ins.extra));
                break;
            }
            case BC_DEL_ITEM: {
                // DEL_ITEM     0       0
                PyLiteObject *key = castobj(kv_pop(vm->stack));
                tobj = castobj(kv_pop(vm->stack));
                pylt_obj_Edelitem(I, tobj, key);
                break;
            }
            case BC_DEL_FORCE:
                // DEL_FORCE    0       0
                //pylt_free(I, castobj(kv_pop(I->vm.stack)));
                break;
            case BC_POP:
                // POP          0       0
                kv_pop(vm->stack);
                break;
            case BC_POPN:
                // POPN         0       num
                kv_popn(vm->stack, ins.extra);
                break;
            case BC_ASSERT:
                // ASSERT       0       extra
                if (ins.extra) ta = castobj(kv_pop(I->vm.stack));
                if (!pylt_obj_istrue(I, castobj(kv_pop(I->vm.stack)))) {
                    if (ins.extra) pl_error(I, pl_static.str.AssertionError, "%s", ta);
                    else pl_error(I, pl_static.str.AssertionError, NULL);
                }
                break;
            case BC_IMPORT_NAME:
                // IMPORT_NAME  0       N
                tobj = castobj(kv_topn(vm->stack, ins.extra - 1)); // name
                tret = pylt_obj_dict_getitem(I, I->inner_module_loaders, tobj);
                if (tret) {
                    PyLiteModuleRegisterFunc func = (PyLiteModuleRegisterFunc)tret;
                    pylt_obj_dict_setitem(I, locals, tobj, castobj((func)(I)));
                } else {
                    // TODO: import from disk
                }
                kv_popn(vm->stack, ins.extra);
                break;
            case BC_UNPACK_SEQ: {
                tret = castobj(kv_pop(vm->stack));
                PyLiteIterObject *iter = pylt_obj_iter_Enew(I, tret);
                if (I->error) break;
                for (PyLiteObject *obj = pylt_obj_iter_next(I, iter); obj; obj = pylt_obj_iter_next(I, iter)) {
                    kv_pushptr(vm->stack, obj);
                }
                break;
            }
            case BC_NOP:
                break;
            case BC_PRINT:
                if (kv_size(I->vm.stack) != 0) {
                    debug_print_obj(I, castobj(kv_top(I->vm.stack)));
                    putchar('\n');
                }
                pl_print(I, "locals: %s\n", castobj(locals));
            case BC_HALT:
                goto _end;
                break;
        }
    }
_end:;
}

PyLiteFrame* pylt_vm_curframe(PyLiteInterpreter *I) {
    return &kv_top(I->vm.frames);
}
