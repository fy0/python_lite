﻿
#include "vm.h"
#include "state.h"
#include "debug.h"
#include "lib/kvec.h"
#include "types/all.h"
#include "mods/builtin.h"

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

void pylt_vm_init(struct PyLiteState *state, PyLiteVM* vm) {
    PyLiteFrame *frame;
    PyLiteTable *locals;
    PyLiteModuleObject *mod;

    kv_init(vm->stack);
    kv_init(vm->frames);

    // first frame
    kv_pushp(PyLiteFrame, vm->frames);
    frame = &kv_A(vm->frames, 0);
    frame->func = NULL;
    kv_init(frame->var_tables);

    // built-in
    kv_push(PyLiteTable*, frame->var_tables, pylt_obj_table_new(state));
    locals = kv_top(frame->var_tables);
    mod = pylt_mods_builtins_register(state);

    pl_int_t k = kho_begin(mod->attrs);
    while (k != kho_end(mod->attrs)) {
        if (kho_exist(mod->attrs, k)) {
            pylt_obj_table_set(locals, kho_key(mod->attrs, k), kho_val(mod->attrs, k));
        }
        ++k;
    }

    // first local
    kv_push(PyLiteTable*, frame->var_tables, pylt_obj_table_new(state));
}

void pylt_vm_call_func(PyLiteState* state, PyLiteFunctionObject *func) {
    PyLiteFrame *frame;
    PyLiteVM *vm = &state->vm;
    int index = kv_size(vm->frames);

    kv_pushp(PyLiteFrame, vm->frames);
    frame = &kv_A(vm->frames, index);
    frame->func = func;
    frame->code = &func->code;
    kv_init(frame->var_tables);

    if (index) {
        kv_copy1(PyLiteTable*, frame->var_tables, kv_A(vm->frames, index - 1).var_tables);
    }

    kv_push(PyLiteTable*, frame->var_tables, pylt_obj_table_new(state));
}


void pylt_vm_load_code(PyLiteState* state, PyLiteCodeSnippetObject *code) {
    PyLiteVM *vm = &state->vm;
    PyLiteFrame *frame = &kv_top(vm->frames);
    frame->func = NULL;
    frame->code = code;
}

// 函数调用检查
//  1 为当前的参数个数小于最少参数个数
//  2 为当前的参数个数多于最少参数个数
// -1 当前对象并非函数类型
int func_call_check(PyLiteObject *obj, int params_num, int *pnum) {
    int params_num_min, params_num_max;

    if (obj->ob_type == PYLT_OBJ_TYPE_FUNCTION) {
        params_num_min = castfunc(obj)->minimal;
        params_num_max = castfunc(obj)->length;
    } else if (obj->ob_type == PYLT_OBJ_TYPE_CFUNCTION) {
        params_num_min = castcfunc(obj)->minimal;
        params_num_max = castcfunc(obj)->length;
    } else return -1;

    if (params_num < params_num_min) {
        if (pnum) *pnum = params_num_min; 
        return 1; 
    }

    if (params_num > params_num_max) {
        if (pnum) *pnum = params_num_max;
        return 2;
    }

    return 0;
}

#define const_obj(__index) castobj(kv_A(code->const_val, (__index)))

void pylt_vm_run(PyLiteState* state, PyLiteCodeSnippetObject *code) {
    PyLiteVM *vm = &state->vm;
    PyLiteTable *locals;
    PyLiteInstruction ins;

    int tnum;
    PyLiteObject *tobj, *tret, *ta, *tb, *tc;
    PyLiteFunctionObject *tfunc;

    if (!code) return;
    pylt_vm_load_code(state, code);
    locals = kv_top(kv_top(vm->frames).var_tables);

    for (pl_uint_t i = 0; ; ++i) {
        ins = kv_A(code->opcodes, i);

        switch (ins.code) {
            case BC_LOADCONST:
                // LOAD_CONST   0       const_id
                kv_pushptr(vm->stack, const_obj(ins.extra));
                break;
            case BC_SET_VAL:
                // SET_VAL      0       const_id
                pylt_obj_table_set(locals, const_obj(ins.extra), castobj(kv_top(state->vm.stack)));
                break;
            case BC_LOAD_VAL:
                // LOAD_VAL     0       const_id
                tobj = pylt_obj_table_get(locals, const_obj(ins.extra));

                if (!tobj) {
                    for (int j = kv_size(kv_top(vm->frames).var_tables) - 2; j >= 0; --j) {
                        tobj = pylt_obj_table_get(kv_A(kv_top(vm->frames).var_tables, j), const_obj(ins.extra));
                        if (tobj) break;
                    }
                }

                if (!tobj) {
                    printf("NameError: name '");
                    debug_print_obj(const_obj(ins.extra));
                    printf("' is not defined\n");
                    return;
                }
                kv_push(uintptr_t, state->vm.stack, (uintptr_t)tobj);
                break;
            case BC_OPERATOR:
                // OPERATOR     0       op
                switch (ins.extra) {
                    case OP_LT: case OP_LE: case OP_GT: case OP_GE: case OP_NE: case OP_EQ:
                    case OP_BITOR: case OP_BITXOR: case OP_BITAND: case OP_LSHIFT: case OP_RSHIFT:
                    case OP_PLUS: case OP_MINUS: case OP_MUL: case OP_MATMUL: case OP_DIV: case OP_FLOORDIV: case OP_MOD: case OP_POW:
                        tb = castobj(kv_pop(state->vm.stack));
                        ta = castobj(kv_pop(state->vm.stack));
                        tret = pylt_obj_op_binary(state, ins.extra, ta, tb);
                        if (!tret) {
                            printf("TypeError: unsupported operand type(s) for %s: '%s' and '%s'\n", pylt_vm_get_op_name(ins.exarg), pylt_obj_type_name_cstr(state, ta), pylt_obj_type_name_cstr(state, tb));
                            return;
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)tret);
                        break;
                    default:
                        ta = castobj(kv_pop(state->vm.stack));
                        tret = pylt_obj_op_unary(state, ins.exarg, ta);
                        if (!tret) {
                            printf("TypeError: bad operand type for unary %s: '%s'\n", pylt_vm_get_op_name(ins.exarg), pylt_obj_type_name_cstr(state, ta));
                            return;
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)tret);
                        break;
                }
                break;
            case BC_NEW_OBJ:
                // NEW_OBJ      type    extra
                switch (ins.exarg) {
                    case PYLT_OBJ_TYPE_ITER:
                        tret = castobj(pylt_obj_iter_new(state, castobj(kv_pop(state->vm.stack))));
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)tret);
                        break;
                    case PYLT_OBJ_TYPE_FUNCTION:
                        tc = castobj(kv_pop(state->vm.stack)); // params
                        tb = castobj(kv_pop(state->vm.stack)); // code
                        ta = castobj(kv_pop(state->vm.stack)); // name
                        tfunc = pylt_obj_func_new(state);

                        memcpy(&castfunc(tret)->code, tb, sizeof(PyLiteCodeSnippetObject));

                        tfunc->length = castlist(tc)->ob_size;
                        tfunc->minimal = castlist(tc)->ob_size;
                        tfunc->names = castlist(tc)->ob_val;
                        tfunc->defaults = NULL;
                        tfunc->doc = NULL;

                        pylt_obj_table_set(locals, ta, castobj(tfunc));
                        break;
                    case PYLT_OBJ_TYPE_SET:
                        tobj = castobj(pylt_obj_set_new(state));
                        for (pl_int_t j = 0; j < ins.extra; ++j) {
                            pylt_obj_set_add(state, castset(tobj), castobj(kv_pop(state->vm.stack)));
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_LIST:
                        tobj = castobj(pylt_obj_list_new_with_size(state, ins.extra));
                        for (pl_uint_t j = ins.extra; j > 0; --j) {
                            pylt_obj_list_append(state, castlist(tobj), castobj(kv_A(state->vm.stack, kv_size(state->vm.stack) - j)));
                        }
                        kv_popn(state->vm.stack, ins.extra);
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_TUPLE:
                        tobj = castobj(pylt_obj_tuple_new(state, ins.extra));
                        memcpy(casttuple(tobj)->ob_val, &kv_topn(vm->stack, ins.extra - 1), sizeof(PyLiteObject*) * ins.extra);
                        kv_popn(state->vm.stack, ins.extra);
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)tobj);
                        break;
                    case PYLT_OBJ_TYPE_DICT:
                        tobj = castobj(pylt_obj_dict_new(state));
                        for (pl_uint_t j = ins.extra; j > 0; j--) {
                            pylt_obj_dict_csetitem(state, castdict(tobj), castobj(kv_topn(vm->stack, j * 2 - 1)), castobj(kv_topn(vm->stack, (j - 1) * 2)));
                        }
                        kv_popn(vm->stack, ins.extra * 2);
                        kv_push(uintptr_t, vm->stack, (uintptr_t)tobj);
                        break;
                }
                break;
            case BC_CALL:
                // BC_CALL      0       params_num
                tobj = castobj(kv_pop(vm->stack)); // 函数对象

                // check
                switch (func_call_check(tobj, ins.extra, &tnum)) {
                    case 1:
                        printf("TypeError: ");
                        debug_print_obj(tobj);
                        printf("() missing %d required positional argument\n", tnum - ins.extra);
                        return;
                    case 2:
                        printf("TypeError: ");
                        debug_print_obj(tobj);
                        printf("() takes %d positional arguments but %d were given\n", tnum, ins.extra);
                        return;
                }

                // set locals
                if (tobj->ob_type == PYLT_OBJ_TYPE_FUNCTION) {
                    kv_top(vm->frames).code_pointer_slot = i;
                    pylt_vm_call_func(state, castfunc(tobj));
                    code = kv_top(vm->frames).code;
                    locals = kv_top(kv_top(vm->frames).var_tables);
                    if (castfunc(tobj)->length > 0) {
                        for (int k = ins.extra - 1; k >= 0; --k) {
                            pylt_obj_table_set(locals, castobj(castfunc(tobj)->names[k]), castobj(kv_pop(state->vm.stack)));
                        }
                    }
                    i = -1;
                } else if (tobj->ob_type == PYLT_OBJ_TYPE_CFUNCTION) {
                    tobj = castcfunc(tobj)->func(state, ins.extra, (PyLiteObject**)(&kv_topn(vm->stack, ins.extra - 1)));
                    kv_popn(vm->stack, ins.exarg);
                    kv_push(uintptr_t, state->vm.stack, (uintptr_t)pylt_obj_none_new(state));
                }
                break;
            case BC_RET:
                // RET          0       0
                kv_pop(vm->frames);
                code = kv_top(vm->frames).code;
                locals = kv_top(kv_top(vm->frames).var_tables);
                i = kv_top(vm->frames).code_pointer_slot;
                break;
            case BC_TEST:
                // TEST         0       jump_offset
                if (!pylt_obj_cistrue(state, castobj(kv_pop(state->vm.stack)))) {
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
                ta = pylt_obj_iter_next(state, castiter(kv_top(state->vm.stack)));
                if (ta == NULL) i += ins.extra - 1;
                else kv_push(uintptr_t, state->vm.stack, (uintptr_t)ta);
                break;
            case BC_DEL_FORCE:
                // DEL_FORCE    0       0
                pylt_free(castobj(kv_pop(state->vm.stack)));
                break;
            case BC_POP:
                // POP          0       0
                kv_pop(state->vm.stack);
                break;
            case BC_GET_ITEM:
            case BC_GET_ITEM_EX:
                // GET_ITEM     0       0
                tb = castobj(kv_pop(state->vm.stack));
                ta = castobj(kv_pop(state->vm.stack));
                tobj = pylt_obj_getitem(state, ta, tb);
                if (!tobj) {
                    printf("KeyError: ");
                    debug_print_obj(tb);
                    putchar('\n');
                    return;
                } else {
                    kv_push(uintptr_t, state->vm.stack, (uintptr_t)tobj);
                }
                break;
            case BC_SET_ITEM:
                // SET_ITEM     0       0
                ta = castobj(kv_pop(state->vm.stack));
                tobj = castobj(kv_pop(state->vm.stack));
                tb = castobj(kv_pop(state->vm.stack));
                if (!pylt_obj_setitem(state, tobj, ta, tb)) {
                    printf("IndexError: list assignment index out of range\n");
                    return;
                }
                kv_pushptr(vm->stack, tb);
                break;
            case BC_GET_ATTR:
            case BC_GET_ATTR_EX:
                break;
            case BC_PRINT:
                if (kv_size(state->vm.stack) != 0) {
                    debug_print_obj(castobj(kv_top(state->vm.stack)));
                    putchar('\n');
                }
                printf("locals: {");
                for (khiter_t it = kho_begin(locals); it < kho_end(locals); ++it) {
                    if (!kho_exist(locals, it)) continue;
                    debug_print_obj(kho_key(locals, it));
                    printf(": ");
                    debug_print_obj(kho_value(locals, it));
                    printf(", ");
                }
                printf("}");
                putchar('\n');
            case BC_HALT:
                goto _end;
                break;
        }
    }
_end:;
}
