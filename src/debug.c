
#include "types/all.h"
#include "utils/misc.h"
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
    wprintf(L"CONST VALS:\n");
    for (unsigned int i = 0; i < pylt_obj_list_count(I, code->const_val); i++) {
        PyLiteObject *obj = pylt_obj_list_getitem(I, code->const_val, i);
        /*wprintf(L"    %-4d %-12s ", i, pylt_api_type_name_cstr(I, obj->ob_type));
        debug_print_obj(I, obj);
        putwchar('\n');*/
        pl_print(I, "   %-4d %-12s %s\n", pylt_obj_int_new(I, i), pylt_api_type_name(I, obj->ob_type), pylt_obj_to_repr(I, obj));
    }
}

void debug_print_opcodes(PyLiteInterpreter *I, PyLiteCodeObject *code) {
    PyLiteInstruction ins;

    wprintf(L"OPCODES:\n");
    for (unsigned int i = 0; i < kv_size(code->opcodes); ++i) {
        ins = kv_A(code->opcodes, i);
        wprintf(L"   L%05d: ", i);
        switch (ins.code) {
            case BC_OPERATOR:
                wprintf(L"   %-15ls %s\n", L"OPERATOR", get_op_name(ins.extra));
                break;
            case BC_SET_VAL:
                wprintf(L"   %-15ls %d", L"SET_VAL", ins.extra);
                //debug_print_obj(I, castobj(kv_A(code->opcodes, ++i)));
                putwchar('\n');
                break;
            case BC_SET_VALX:
                wprintf(L"   %-15ls %d %-3d\n", L"SET_VALX", ins.extra, ins.exarg);
                break;
            case BC_LOAD_VAL:
                wprintf(L"   %-15ls %d", L"LOAD_VAL", ins.extra);
                putwchar('\n');
                break;
            case BC_LOAD_VAL_:
                wprintf(L"   %-15ls %d", L"LOAD_VAL_", ins.extra);
                putwchar('\n');
                break;
            case BC_LOADNONE:
                wprintf(L"   %-15ls\n", L"LOADNONE");
                break;
            case BC_LOADCONST:
                wprintf(L"   %-15ls %d\n", L"LOADCONST", ins.extra);
                break;
            case BC_LOADLOCALS:
                wprintf(L"   %-15ls\n", L"LOADLOCALS");
                break;
            case BC_NEW_OBJ:
                wprintf(L"   %-15ls ", L"NEW_OBJ");
                pylt_api_output_str(I, pylt_api_type_name(I, ins.exarg));
                wprintf(L" %d\n", ins.extra);
                break;
            case BC_CALL:
                wprintf(L"   %-15ls %d", L"CALL", ins.extra);
                //debug_print_obj(I, castobj(kv_A(code->opcodes, ++i)));
                putwchar('\n');
                break;
            case BC_RET:
                wprintf(L"   %-15ls\n", L"RET");
                break;
            case BC_TEST:
                wprintf(L"   %-15ls %-3d  ", L"TEST", ins.extra);
                putwchar('\n');
                break;
            case BC_JMP:
                wprintf(L"   %-15ls %-3d  ", L"JMP", ins.extra);
                putwchar('\n');
                break;
            case BC_JMP_BACK:
                wprintf(L"   %-15ls %-3d  ", L"JMP_BACK", ins.extra);
                putwchar('\n');
                break;
            case BC_FORITER:
                wprintf(L"   %-15ls %-3d  ", L"FORITER", ins.extra);
                putwchar('\n');
                break;
            case BC_DEL_FORCE:
                wprintf(L"   %-15ls\n", L"DEL_FORCE");
                break;
            case BC_POP:
                wprintf(L"   %-15ls\n", L"POP");
                break;
            case BC_POPN:
                wprintf(L"   %-15ls %d\n", L"POPN", ins.extra);
                break;
            case BC_GET_ITEM:
                wprintf(L"   %-15ls\n", L"GET_ITEM");
                break;
            case BC_GET_ITEM_:
                wprintf(L"   %-15ls\n", L"GET_ITEM_");
                break;
            case BC_SET_ITEM:
                wprintf(L"   %-15ls\n", L"SET_ITEM");
                break;
            case BC_GET_ATTR:
                wprintf(L"   %-15ls %d\n", L"GET_ATTR", ins.extra);
                break;
            case BC_GET_ATTR_:
                wprintf(L"   %-15ls %d\n", L"GET_ATTR_", ins.extra);
                break;
            case BC_SET_ATTR:
                wprintf(L"   %-15ls %d\n", L"SET_ATTR", ins.extra);
                break;
            case BC_GET_SLICE:
                wprintf(L"   %-15ls\n", L"GET_SLICE");
                break;
            case BC_GET_SLICE_:
                wprintf(L"   %-15ls\n", L"GET_SLICE_");
                break;
            case BC_SET_SLICE:
                wprintf(L"   %-15ls\n", L"SET_SLICE");
                break;
            case BC_DEL_NAME:
                wprintf(L"   %-15ls %d\n", L"DEL_NAME", ins.extra);
                break;
            case BC_DEL_ATTR:
                wprintf(L"   %-15ls %d\n", L"DEL_ATTR", ins.extra);
                break;
            case BC_DEL_ITEM:
                wprintf(L"   %-15ls\n", L"DEL_ITEM");
                break;
            case BC_DEL_SLICE:
                wprintf(L"   %-15ls\n", L"DEL_SLICE");
                break;
            case BC_ASSERT:
                wprintf(L"   %-15ls\n", L"ASSERT");
                break;
            case BC_IMPORT_NAME:
                wprintf(L"   %-15ls %d\n", L"IMPORT_NAME", ins.extra);
                break;
            case BC_UNPACK_SEQ:
                wprintf(L"   %-15ls %d\n", L"UNPACK_SEQ", ins.extra);
                break;
            case BC_PRINT:
                wprintf(L"   %-15ls\n", L"PRINT");
                break;
            case BC_NOP:
                wprintf(L"   %-15ls\n", L"NOP");
                break;
            case BC_HALT:
                wprintf(L"   %-15ls\n", L"HALT");
                break;
        }
    }
}

void debug_test_lexer(PyLiteInterpreter *I, PyLiteFile *input) {
    I->lexer->input = input;

    for (;;) {
        int code = pylt_lex_next(I->lexer);
        if (code) {
            wprintf(L"ERROR: %d\n", code);
            break;
        }
        if (I->lexer->token.val < FIRST_TOKEN) wprintf(L"[%d] %c\n", I->lexer->linenumber, I->lexer->token.val);
        else {
            switch (I->lexer->token.val) {
                case TK_INT: case TK_FLOAT:
                    wprintf(L"[%d] %s: ", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val));
                    debug_print_obj(I, I->lexer->token.obj);
                    //raw_str_print(&I->lexer->token.str);
                    putwchar('\n');
                    break;
                case TK_BYTES: case TK_STRING: case TK_NAME:
                    wprintf(L"[%d] %s: ", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val));
                    //raw_str_print(&I->lexer->token.str);
                    debug_print_obj(I, I->lexer->token.obj);
                    putwchar('\n');
                    break;
                case TK_INDENT: case TK_DEDENT:
                    wprintf(L"[%d] %s: %d\n", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val), I->lexer->current_indent);
                    break;
                default:
                    wprintf(L"[%d] %s\n", I->lexer->linenumber, pylt_lex_get_token_name(I->lexer->token.val));
            }
        }
        if (I->lexer->token.val == TK_END) break;
    }
}
