
#include "vm.h"
#include "state.h"
#include "debug.h"
#include "lib/kvec.h"
#include "types/number.h"

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
    ParserState *ps = &state->ps;
    PyLiteObject *a, *b, *ret;
    size_t op;

    for (unsigned int i = 0; i < kv_size(ps->opcodes); i++) {
        switch (kv_A(ps->opcodes, i)) {
            case BC_LOADCONST:
                //printf("   %-15s %d\n", "LOADCONST", );
                //printf("%d\n", kv_A(ps->opcodes, ++i));
                kv_push(size_t, state->vm.stack, (size_t)kv_A(ps->const_val, kv_A(ps->opcodes, ++i)-1));
                break;
            case BC_OPERATOR:
                //printf("   %-15s %s\n", "OPERATOR", get_op_name(kv_A(ps->opcodes, ++i)));
                op = kv_A(ps->opcodes, ++i);
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
                        kv_push(size_t, state->vm.stack, (size_t)ret);
                        break;
                    default:
                        a = castobj(kv_pop(state->vm.stack));
                        ret = pylt_obj_op_unary(state, op, a);
                        if (!ret) {
                            printf("TTypeError: bad operand type for unary %s: '%s'\n", pylt_vm_get_op_name(op), pylt_obj_type_name_cstr(state, a));
                            return;
                        }
                        kv_push(size_t, state->vm.stack, (size_t)ret);
                        break;
                }
                break;
            case BC_PRINT:
                debug_print_obj(castobj(kv_A(state->vm.stack, kv_size(state->vm.stack)-1)));
                putchar('\n');
        }
    }
}
