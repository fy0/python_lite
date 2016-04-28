
#include "types/all.h"
#include "debug.h"
#include "lexer.h"
#include "vm.h"
#include "api.h"

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
            printf("<function ");
            debug_print_obj(castobj(castfunc(obj)->info.name));
            printf(">");
            break;
        case PYLT_OBJ_TYPE_CFUNCTION:
            printf("<cfunction ");
            debug_print_obj(castobj(castcfunc(obj)->info.name));
            printf(">");
            break;
        case PYLT_OBJ_TYPE_MODULE:
            break;
        case PYLT_OBJ_TYPE_TYPE:
            printf("<class '%s'>", pylt_api_type_name(casttype(obj)->ob_reftype));
            break;
        case PYLT_OBJ_TYPE_TUPLE:
            printf("(");
            if (castlist(obj)->ob_size) {
                for (int i = 0; i < casttuple(obj)->ob_size - 1; ++i) {
                    debug_print_obj(casttuple(obj)->ob_val[i]);
                    printf(", ");
                }
                debug_print_obj(casttuple(obj)->ob_val[casttuple(obj)->ob_size - 1]);
            }
            if (casttuple(obj)->ob_size == 1) printf(",)");
            else printf(")");
            break;
        case PYLT_OBJ_TYPE_LIST:
            printf("[");
            if (castlist(obj)->ob_size) {
                for (pl_uint_t i = 0; i < castlist(obj)->ob_size - 1; ++i) {
                    debug_print_obj(castlist(obj)->ob_val[i]);
                    printf(", ");
                }
                debug_print_obj(castlist(obj)->ob_val[castlist(obj)->ob_size - 1]);
            }
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
            printf("<code>");
            break;
        case PYLT_OBJ_TYPE_NONE:
            printf("None");
            break;
        case PYLT_OBJ_TYPE_PROP:
            printf("<property>");
            break;
        default:
            if (obj->ob_type > PYLT_OBJ_BUILTIN_TYPE_NUM) {
                ;
            }
    }
}

void debug_print_const_vals(ParserState *ps) {
    printf("CONST VALS:\n");
    for (unsigned int i = 0; i < kv_size(ps->info->code->const_val); i++) {
        PyLiteObject *obj = kv_A(ps->info->code->const_val, i);
        printf("   %-4d %-8s ", i, pylt_api_type_name(obj->ob_type));
        debug_print_obj(obj);
        putchar('\n');
    }
}

void debug_print_opcodes(ParserState *ps) {
    PyLiteCodeSnippetObject *code = ps->info->code;
    PyLiteInstruction ins;

    printf("OPCODES:\n");
    for (unsigned int i = 0; i < kv_size(code->opcodes); ++i) {
        ins = kv_A(code->opcodes, i);
        printf("   L%05d: ", i);
        switch (ins.code) {
            case BC_OPERATOR:
                printf("   %-15s %s\n", "OPERATOR", get_op_name(ins.extra));
                break;
            case BC_SET_VAL:
                printf("   %-15s %d", "SET_VAL", ins.extra);
                //debug_print_obj(castobj(kv_A(code->opcodes, ++i)));
                putchar('\n');
                break;
            case BC_LOAD_VAL:
                printf("   %-15s %d", "LOAD_VAL", ins.extra);
                putchar('\n');
                break;
            case BC_LOAD_VAL_EX:
                printf("   %-15s %d", "LOAD_VAL_EX", ins.extra);
                putchar('\n');
                break;
            case BC_LOADCONST:
                printf("   %-15s %d\n", "LOADCONST", ins.extra);
                break;
            case BC_LOADLOCALS:
                printf("   %-15s\n", "LOADLOCALS");
                break;
            case BC_NEW_OBJ:
                printf("   %-15s %s %d\n", "NEW_OBJ", pylt_api_type_name(ins.exarg), ins.extra);
                break;
            case BC_CALL:
                printf("   %-15s %d", "CALL", ins.extra);
                //debug_print_obj(castobj(kv_A(code->opcodes, ++i)));
                putchar('\n');
                break;
            case BC_RET:
                printf("   %-15s\n", "RET");
                break;
            case BC_TEST:
                printf("   %-15s %-3d  ", "TEST", ins.extra);
                putchar('\n');
                break;
            case BC_JMP:
                printf("   %-15s %-3d  ", "JMP", ins.extra);
                putchar('\n');
                break;
            case BC_JMP_BACK:
                printf("   %-15s %-3d  ", "JMP_BACK", ins.extra);
                putchar('\n');
                break;
            case BC_FORITER:
                printf("   %-15s %-3d  ", "FORITER", ins.extra);
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
            case BC_GET_ITEM_EX:
                printf("   %-15s\n", "GET_ITEM_EX");
                break;
            case BC_SET_ITEM:
                printf("   %-15s\n", "SET_ITEM");
                break;
            case BC_GET_ATTR:
                printf("   %-15s %d\n", "GET_ATTR", ins.extra);
                break;
            case BC_GET_ATTR_EX:
                printf("   %-15s %d\n", "GET_ATTR_EX", ins.extra);
                break;
            case BC_SET_ATTR:
                printf("   %-15s %d\n", "SET_ATTR", ins.extra);
                break;
            case BC_ASSERT:
                printf("   %-15s\n", "ASSERT");
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
