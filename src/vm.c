
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

void pylt_vm_init(PyLiteVM* vm) {
    kv_init(vm->stack);
}

void pylt_vm_run(PyLiteState* state) {
    PyLiteFunctionObject *func = state->func;
    PyLiteObject *a, *b, *ret;
    uintptr_t op;
    int tmp;

    for (unsigned int i = 0; i < kv_size(func->opcodes); i++) {
        switch (kv_A(func->opcodes, i)) {
            case BC_LOADCONST:
                //printf("   %-15s %d\n", "LOADCONST", );
                //printf("%d\n", kv_A(func->opcodes, ++i));
                kv_push(uintptr_t, state->vm.stack, (uintptr_t)kv_A(func->const_val, kv_A(func->opcodes, ++i)-1));
                break;
            case BC_SET_VAL:
                pylt_obj_table_set(state->func->locals, castobj(kv_A(func->opcodes, ++i)), castobj(kv_pop(state->vm.stack)));
                break;
            case BC_LOAD_VAL:
                ret = pylt_obj_table_get(state->func->locals, castobj(kv_A(func->opcodes, ++i)));
                if (!ret) {
                    printf("NameError: name '");
                    debug_print_obj(castobj(kv_A(func->opcodes, i)));
                    printf("' is not defined\n");
                    return;
                }
                kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                break;
            case BC_OPERATOR:
                //printf("   %-15s %s\n", "OPERATOR", get_op_name(kv_A(func->opcodes, ++i)));
                op = kv_A(func->opcodes, ++i);
                switch (op) {
                    case OP_LT: case OP_LE: case OP_GT: case OP_GE: case OP_NE: case OP_EQ:
                    case OP_BITOR: case OP_BITXOR: case OP_BITAND: case OP_LSHIFT: case OP_RSHIFT:
                    case OP_PLUS: case OP_MINUS: case OP_MUL : case OP_MATMUL: case OP_DIV: case OP_FLOORDIV: case OP_MOD: case OP_POW :
                        b = castobj(kv_pop(state->vm.stack));
                        a = castobj(kv_pop(state->vm.stack));
                        ret = pylt_obj_op_binary(state, op, a, b);
                        if (!ret) {
                            printf("TypeError: unsupported operand type(s) for %s: '%s' and '%s'\n", pylt_vm_get_op_name(op), pylt_obj_type_name_cstr(state, a), pylt_obj_type_name_cstr(state, b));
                            return;
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                    default:
                        a = castobj(kv_pop(state->vm.stack));
                        ret = pylt_obj_op_unary(state, op, a);
                        if (!ret) {
                            printf("TypeError: bad operand type for unary %s: '%s'\n", pylt_vm_get_op_name(op), pylt_obj_type_name_cstr(state, a));
                            return;
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                }
                break;
            case BC_NEW_OBJ:
                switch (kv_A(func->opcodes, ++i)) {
                    case PYLT_OBJ_TYPE_SET:
                        ret = castobj(pylt_obj_set_new(state));
                        tmp = kv_A(func->opcodes, ++i);
                        for (int j = 0; j < tmp; j++) {
                            pylt_obj_set_add(state, castset(ret), castobj(kv_pop(state->vm.stack)));
                        }
                        kv_push(uintptr_t, state->vm.stack, (uintptr_t)ret);
                        break;
                    case PYLT_OBJ_TYPE_DICT:
                        ret = castobj(pylt_obj_dict_new(state));
                        break;
                }
                break;
            case BC_CALL:
                //pylt_mods_builtins_print(state, kv_A(func->opcodes, ++i));
                ++i; 
                break;
            case BC_PRINT:
                if (kv_size(state->vm.stack) != 0) {
                    debug_print_obj(castobj(kv_A(state->vm.stack, kv_size(state->vm.stack) - 1)));
                    putchar('\n');
                }
                printf("locals: {");
                for (khiter_t it = kho_begin(state->func->locals); it < kho_end(state->func->locals); ++it) {
                    if (!kho_exist(state->func->locals, it)) continue;
                    debug_print_obj(kho_key(state->func->locals, it));
                    printf(": ");
                    debug_print_obj(kho_value(state->func->locals, it));
                    printf(", ");
                }
                printf("}");
                putchar('\n');
        }
    }
}
