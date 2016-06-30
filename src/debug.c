
#include "types/all.h"
#include "debug.h"
#include "lexer.h"
#include "vm.h"
#include "api.h"
#include "state.h"

const char* get_op_name(uint32_t val) {
    return pylt_vm_get_op_name(val);
}

void debug_print_obj(PyLiteState *state, PyLiteObject *obj) {
    pylt_api_output_str(state, pylt_obj_to_repr(state, obj));
}

void debug_print_const_vals(PyLiteState *state, ParserState *ps) {
    printf("CONST VALS:\n");
    for (unsigned int i = 0; i < pylt_obj_list_count(state, ps->info->code->const_val); i++) {
        PyLiteObject *obj = pylt_obj_list_getitem(state, ps->info->code->const_val, i);
        printf("   %-4d %-8s ", i, pylt_api_type_name_cstr(state, obj->ob_type));
        debug_print_obj(state, obj);
        putchar('\n');
    }
}

void debug_print_opcodes(PyLiteState *state, ParserState *ps) {
    PyLiteCodeObject *code = ps->info->code;
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
                //debug_print_obj(state, castobj(kv_A(code->opcodes, ++i)));
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
                printf("   %-15s %s %d\n", "NEW_OBJ", pylt_api_type_name_cstr(NULL, ins.exarg), ins.extra);
                break;
            case BC_CALL:
                printf("   %-15s %d", "CALL", ins.extra);
                //debug_print_obj(state, castobj(kv_A(code->opcodes, ++i)));
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
            case BC_IMPORT_NAME:
                printf("   %-15s %d\n", "IMPORT_NAME", ins.extra);
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

void debug_test_lexer(PyLiteState *state, StringStream *ss) {
    state->lexer->ss = ss;

    for (;;) {
        int code = pylt_lex_next(state->lexer);
        if (code) {
            printf("ERROR: %d\n", code);
            break;
        }
        if (state->lexer->token.val < FIRST_TOKEN) printf("[%d] %c\n", state->lexer->linenumber, state->lexer->token.val);
        else {
            switch (state->lexer->token.val) {
                case TK_INT: case TK_FLOAT:
                    printf("[%d] %s: ", state->lexer->linenumber, pylt_lex_get_token_name(state->lexer->token.val));
                    debug_print_obj(state, state->lexer->token.obj);
                    //raw_str_print(&state->lexer->token.str);
                    putchar('\n');
                    break;
                case TK_BYTES: case TK_STRING: case TK_NAME:
                    printf("[%d] %s: ", state->lexer->linenumber, pylt_lex_get_token_name(state->lexer->token.val));
                    //raw_str_print(&state->lexer->token.str);
                    debug_print_obj(state, state->lexer->token.obj);
                    putchar('\n');
                    break;
                case TK_INDENT: case TK_DEDENT:
                    printf("[%d] %s: %d\n", state->lexer->linenumber, pylt_lex_get_token_name(state->lexer->token.val), state->lexer->current_indent);
                    break;
                default:
                    printf("[%d] %s\n", state->lexer->linenumber, pylt_lex_get_token_name(state->lexer->token.val));
            }
        }
        if (state->lexer->token.val == TK_END) break;
    }
}
