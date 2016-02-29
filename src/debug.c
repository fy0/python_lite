
#include "debug.h"
#include "lexer.h"
#include "vm.h"
#include "types/all.h"

const char* get_op_name(uint32_t val) {
    return pylt_vm_get_op_name(val);
}

void debug_print_obj(PyLiteObject *obj) {
    switch (obj->ob_type) {
        case PYLT_OBJ_TYPE_INT: printf("%d", castint(obj)->ob_val); break;
        case PYLT_OBJ_TYPE_FLOAT: printf("%f", castfloat(obj)->ob_val); break;
        case PYLT_OBJ_TYPE_BOOL: printf((castbool(obj)->ob_val == 0) ? "False" : "True"); break;
        case PYLT_OBJ_TYPE_BYTES: printf("b'%.*s'", castbytes(obj)->ob_size, castbytes(obj)->ob_val); break;
        case PYLT_OBJ_TYPE_STR:
            putchar('\'');
            for (size_t i = 0; i < caststr(obj)->ob_size; i++) {
                putcode(caststr(obj)->ob_val[i]);
            }
            putchar('\'');
            break;
        case PYLT_OBJ_TYPE_FUNCTION:
        case PYLT_OBJ_TYPE_MODULE:
        case PYLT_OBJ_TYPE_TYPE:
        case PYLT_OBJ_TYPE_TUPLE:
        case PYLT_OBJ_TYPE_SET:
            printf("{");
            for (khiter_t it = kho_begin(castset(obj)->ob_val); it < kho_end(castset(obj)->ob_val); ++it) {
                if (!kho_exist(castset(obj)->ob_val, it)) continue;
                debug_print_obj(kho_key(castset(obj)->ob_val, it));
                if (it < pylt_obj_set_len(NULL, castset(obj))) printf(", ");
            }
            printf("}");
            break;
        case PYLT_OBJ_TYPE_DICT:
        default:
            if (obj->ob_type >= PYLT_OBJ_TYPE_CLASS) {
            }
    }
}

void debug_print_const_vals(ParserState *ps) {
    printf("CONST VALS:\n");
    for (unsigned int i = 0; i < kv_size(ps->func->const_val); i++) {
        PyLiteObject *obj = kv_A(ps->func->const_val, i);
        printf("   %-4d %-8s ", i + 1, pylt_type_name(obj->ob_type));
        debug_print_obj(obj);
        putchar('\n');
    }
}

void debug_print_opcodes(ParserState *ps) {
    printf("OPCODES:\n");
    for (unsigned int i = 0; i < kv_size(ps->func->opcodes); i++) {
        switch (kv_A(ps->func->opcodes, i)) {
            case BC_LOADCONST:
                printf("   %-15s %d\n", "LOADCONST", kv_A(ps->func->opcodes, ++i));
                break;
            case BC_OPERATOR:
                printf("   %-15s %s\n", "OPERATOR", get_op_name(kv_A(ps->func->opcodes, ++i)));
                break;
            case BC_PRINT:
                printf("   %-15s\n", "PRINT");
                break;
        }
    }
}
