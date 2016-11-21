
#include "types/all.h"
#include "debug.h"
#include "lexer.h"
#include "vm.h"
#include "api.h"
#include "intp.h"

const char* get_op_name(uint32_t val) {
    return pylt_vm_get_op_name(val);
}

void debug_print_obj(PyLiteInterpreter *I, PyLiteObject *obj) {
    pylt_api_output_str(I, pylt_obj_to_repr(I, obj));
}

void debug_print_const_vals(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    printf("CONST VALS:\n");
    for (unsigned int i = 0; i < pylt_obj_list_count(I, code->const_val); i++) {
        PyLiteObject *obj = pylt_obj_list_getitem(I, code->const_val, i);
        /*printf("    %-4d %-12s ", i, pylt_api_type_name_cstr(I, obj->ob_type));
        debug_print_obj(I, obj);
        putchar('\n');*/
        pl_print(I, "   %-4d %-12s %s\n", pylt_obj_int_new(I, i), pylt_api_type_name(I, obj->ob_type), pylt_obj_to_repr(I, obj));
    }
}

void debug_print_opcodes(PyLiteInterpreter *I, PyLiteCodeObject *code) {
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
                //debug_print_obj(I, castobj(kv_A(code->opcodes, ++i)));
                putchar('\n');
                break;
            case BC_SET_VALX:
                printf("   %-15s %d %-3d\n", "SET_VALX", ins.extra, ins.exarg);
                break;
            case BC_LOAD_VAL:
                printf("   %-15s %d", "LOAD_VAL", ins.extra);
                putchar('\n');
                break;
            case BC_LOAD_VAL_:
                printf("   %-15s %d", "LOAD_VAL_", ins.extra);
                putchar('\n');
                break;
            case BC_LOADNONE:
                printf("   %-15s\n", "LOADNONE");
                break;
            case BC_LOADCONST:
                printf("   %-15s %d\n", "LOADCONST", ins.extra);
                break;
            case BC_LOADLOCALS:
                printf("   %-15s\n", "LOADLOCALS");
                break;
            case BC_NEW_OBJ:
                printf("   %-15s ", "NEW_OBJ");
                pylt_api_output_str(I, pylt_api_type_name(I, ins.exarg));
                printf(" %d\n", ins.extra);
                break;
            case BC_CALL:
                printf("   %-15s %d", "CALL", ins.extra);
                //debug_print_obj(I, castobj(kv_A(code->opcodes, ++i)));
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
            case BC_POPN:
                printf("   %-15s %d\n", "POPN", ins.extra);
                break;
            case BC_GET_ITEM:
                printf("   %-15s\n", "GET_ITEM");
                break;
            case BC_GET_ITEM_:
                printf("   %-15s\n", "GET_ITEM_");
                break;
            case BC_SET_ITEM:
                printf("   %-15s\n", "SET_ITEM");
                break;
            case BC_GET_ATTR:
                printf("   %-15s %d\n", "GET_ATTR", ins.extra);
                break;
            case BC_GET_ATTR_:
                printf("   %-15s %d\n", "GET_ATTR_", ins.extra);
                break;
            case BC_SET_ATTR:
                printf("   %-15s %d\n", "SET_ATTR", ins.extra);
                break;
            case BC_GET_SLICE:
                printf("   %-15s\n", "GET_SLICE");
                break;
            case BC_GET_SLICE_:
                printf("   %-15s\n", "GET_SLICE_");
                break;
            case BC_SET_SLICE:
                printf("   %-15s\n", "SET_SLICE");
                break;
            case BC_ASSERT:
                printf("   %-15s\n", "ASSERT");
                break;
            case BC_IMPORT_NAME:
                printf("   %-15s %d\n", "IMPORT_NAME", ins.extra);
                break;
            case BC_UNPACK_SEQ:
                printf("   %-15s %d\n", "UNPACK_SEQ", ins.extra);
                break;
            case BC_PRINT:
                printf("   %-15s\n", "PRINT");
                break;
            case BC_NOP:
                printf("   %-15s\n", "NOP");
                break;
            case BC_HALT:
                printf("   %-15s\n", "HALT");
                break;
        }
    }
}

void debug_test_lexer(PyLiteInterpreter *I, StringStream *ss) {
    I->lexer->ss = ss;

    for (;;) {
        int code = pylt_lex_next(I->lexer);
        if (code) {
            printf("ERROR: %d\n", code);
            break;
        }
        if (I->lexer->token.val < FIRST_TOKEN) printf("[%d] %c\n", I->lexer->linenumber, I->lexer->token.val);
        else {
            switch (I->lexer->token.val) {
                case TK_INT: case TK_FLOAT:
                    printf("[%d] %s: ", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val));
                    debug_print_obj(I, I->lexer->token.obj);
                    //raw_str_print(&I->lexer->token.str);
                    putchar('\n');
                    break;
                case TK_BYTES: case TK_STRING: case TK_NAME:
                    printf("[%d] %s: ", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val));
                    //raw_str_print(&I->lexer->token.str);
                    debug_print_obj(I, I->lexer->token.obj);
                    putchar('\n');
                    break;
                case TK_INDENT: case TK_DEDENT:
                    printf("[%d] %s: %d\n", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val), I->lexer->current_indent);
                    break;
                default:
                    printf("[%d] %s\n", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val));
            }
        }
        if (I->lexer->token.val == TK_END) break;
    }
}
