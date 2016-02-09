
#include "debug.h"
#include "lexer.h"
#include "vm.h"
#include "types/bool.h"
#include "types/number.h"

const char* get_op_name(uint32_t val) {
    return pylt_vm_get_op_name(val);
}

void debug_print_const_vals(ParserState *ps) {
    printf("CONST VALS:\n");
    for (int i = 0; i < kv_size(ps->const_val); i++) {
        PyLiteObject *obj = kv_A(ps->const_val, i);
        printf("   %-4d %-8s ", i+1, pylt_obj_type_name(obj->ob_type));
        switch (obj->ob_type) {
            case PYLT_OBJ_TYPE_INT: printf("%d", castint(obj)->ob_val); break;
            case PYLT_OBJ_TYPE_FLOAT: printf("%f", castfloat(obj)->ob_val); break;
            case PYLT_OBJ_TYPE_BOOL: printf((castbool(obj)->ob_val == 0) ? "False" : "True"); break;
            case PYLT_OBJ_TYPE_BYTES:
            case PYLT_OBJ_TYPE_STR:
            case PYLT_OBJ_TYPE_FUNCTION:
            case PYLT_OBJ_TYPE_MODULE:
            case PYLT_OBJ_TYPE_TYPE:
            case PYLT_OBJ_TYPE_TUPLE:
            case PYLT_OBJ_TYPE_SET:
            case PYLT_OBJ_TYPE_DICT:
            default:
                if (obj->ob_type >= PYLT_OBJ_TYPE_CLASS) {
                }
        }
        putchar('\n');
    }
}

void debug_print_opcodes(ParserState *ps) {
    printf("OPCODES:\n");
    for (int i = 0; i < kv_size(ps->opcodes); i++) {
        switch (kv_A(ps->opcodes, i)) {
            case BC_LOADCONST:
                printf("   %-15s %d\n", "LOADCONST", kv_A(ps->opcodes, ++i));
                break;
            case BC_OPERATOR:
                printf("   %-15s %s\n", "OPERATOR", get_op_name(kv_A(ps->opcodes, ++i)));
                break;
        }
    }
}
