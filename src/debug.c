
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
            printf("<function>");
            break;
        case PYLT_OBJ_TYPE_MODULE:
        case PYLT_OBJ_TYPE_TYPE:
        case PYLT_OBJ_TYPE_TUPLE:
            printf("(");
            for (int i = 0; i < casttuple(obj)->ob_size - 1; ++i) {
                debug_print_obj(casttuple(obj)->ob_val[i]);
                printf(", ");
            }
            debug_print_obj(casttuple(obj)->ob_val[casttuple(obj)->ob_size - 1]);
            if (casttuple(obj)->ob_size == 1) printf(",)");
            else printf(")");
            break;
        case PYLT_OBJ_TYPE_LIST:
            printf("[");
            for (int i = 0; i < castlist(obj)->ob_size - 1; ++i) {
                debug_print_obj(castlist(obj)->ob_val[i]);
                printf(", ");
            }
            debug_print_obj(castlist(obj)->ob_val[castlist(obj)->ob_size - 1]);
            printf("]");
            break;
        case PYLT_OBJ_TYPE_SET:
            printf("{");
            for (khiter_t it = kho_begin(castset(obj)->ob_val); it < kho_end(castset(obj)->ob_val); ++it) {
                if (!kho_exist(castset(obj)->ob_val, it)) continue;
                debug_print_obj(kho_key(castset(obj)->ob_val, it));
                printf(", ");
            }
            printf("}");
            break;
        case PYLT_OBJ_TYPE_DICT:
            printf("{");
            for (pl_int_t it = pylt_obj_dict_begin(NULL, castdict(obj)); it != pylt_obj_dict_end(NULL, castdict(obj)); pylt_obj_dict_next(NULL, castdict(obj), &it)) {
                debug_print_obj(pylt_obj_dict_itemkey(NULL, castdict(obj), it));
                printf(": ");
                debug_print_obj(pylt_obj_dict_itemvalue(NULL, castdict(obj), it));
                printf(", ");
            }
            printf("}");
            break;
        case PYLT_OBJ_TYPE_CODESNIPPET:
            printf("Code");
            break;
        case PYLT_OBJ_TYPE_NONE:
            printf("None");
            break;
        default:
            if (obj->ob_type >= PYLT_OBJ_TYPE_CLASS) {
                ;
            }
    }
}

void debug_print_const_vals(ParserState *ps) {
    printf("CONST VALS:\n");
    for (unsigned int i = 0; i < kv_size(ps->info->code->const_val); i++) {
        PyLiteObject *obj = kv_A(ps->info->code->const_val, i);
        printf("   %-4d %-8s ", i + 1, pylt_type_name(obj->ob_type));
        debug_print_obj(obj);
        putchar('\n');
    }
}

void debug_print_opcodes(ParserState *ps) {
    int a, b;
    PyLiteCodeSnippetObject *code = ps->info->code;

    printf("OPCODES:\n");
    for (unsigned int i = 0; i < kv_size(code->opcodes); i++) {
        printf("   L%05d: ", i);
        switch (kv_A(code->opcodes, i)) {
            case BC_OPERATOR:
                printf("   %-15s %s\n", "OPERATOR", get_op_name(kv_A(code->opcodes, ++i)));
                break;
            case BC_SET_VAL:
                printf("   %-15s ", "SET_VAL");
                debug_print_obj(castobj(kv_A(code->opcodes, ++i)));
                putchar('\n');
                break;
            case BC_LOAD_VAL:
                printf("   %-15s ", "LOAD_VAL");
                debug_print_obj(castobj(kv_A(code->opcodes, ++i)));
                putchar('\n');
                break;
            case BC_LOADCONST:
                printf("   %-15s %d\n", "LOADCONST", kv_A(code->opcodes, ++i));
                break;
            case BC_NEW_OBJ:
                a = kv_A(code->opcodes, ++i);
                printf("   %-15s %s\n", "NEW_OBJ", pylt_type_name(a));
                break;
            case BC_NEW_OBJ_EXTRA:
                a = kv_A(code->opcodes, ++i);
                b = kv_A(code->opcodes, ++i);
                printf("   %-15s %s %d\n", "NEW_OBJ_EX", pylt_type_name(a), b);
                break;
            case BC_CALL:
                printf("   %-15s %-3d  ", "CALL", kv_A(code->opcodes, ++i));
                debug_print_obj(castobj(kv_A(code->opcodes, ++i)));
                putchar('\n');
                break;
            case BC_RET:
                printf("   %-15s\n", "RET");
                break;
            case BC_TEST:
                printf("   %-15s %-3d  ", "TEST", kv_A(code->opcodes, ++i));
                putchar('\n');
                break;
            case BC_JMP:
                printf("   %-15s %-3d  ", "JMP", kv_A(code->opcodes, ++i));
                putchar('\n');
                break;
            case BC_JMP_BACK:
                printf("   %-15s %-3d  ", "JMP_BACK", kv_A(code->opcodes, ++i));
                putchar('\n');
                break;
            case BC_FORITER:
                printf("   %-15s %-3d  ", "FORITER", kv_A(code->opcodes, ++i));
                putchar('\n');
                break;
            case BC_DEL_FORCE:
                printf("   %-15s\n", "DEL_FORCE");
                break;
            case BC_POP:
                printf("   %-15s\n", "POP");
                break;
            case BC_GET_ITEM:
                printf("   %-15s\n", "GET_ITEM");
                break;
            case BC_SET_ITEM:
                printf("   %-15s\n", "SET_ITEM");
                break;
            case BC_GET_ATTR:
                printf("   %-15s\n", "GET_ATTR");
                break;
            case BC_SET_ATTR:
                printf("   %-15s\n", "SET_ATTR");
                break;
            case BC_PRINT:
                printf("   %-15s\n", "PRINT");
                break;
            case BC_HALT:
                printf("   %-15s\n", "HALT");
                break;
        }
    }
}
