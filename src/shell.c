﻿
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "io.h"
#include "debug.h"
#include "vm.h"
#include "state.h"

#include "types/object.h"


#ifdef SHELL

int main(int argc,char* argv[])
{
    int size;
    char *buf = read_file("test.py", &size);

    StringStream *ss = ss_new(buf, size);
    printf(buf);
    putchar('\n');

    LexState ls;
    pylt_lex_init(&ls, ss);

    PyLiteState state;
    ParserState ps;
    pylt_parser_init(&ps, &state, &ls);
    parse(&ps);

    debug_print_const_vals(&ps);
    debug_print_opcodes(&ps);

    state.ps = ps;
    pylt_vm_init(&state.vm);
    pylt_vm_run(&state);

    system("pause");
    return 0;

    for (;;) {
        int code = pylt_lex_next(&ls);
        if (code) {
            printf("ERROR: %d\n", code);
            break;
        }
        if (ls.token.val < FIRST_TOKEN) printf("[%d] %c\n", ls.linenumber, ls.token.val);
        else {
            switch (ls.token.val) {
                case TK_INT: case TK_FLOAT:
                    printf("[%d] %s: ", ls.linenumber, pylt_lex_get_token_name(ls.token.val));
                    raw_str_print(&ls.token.str);
                    putchar('\n');
                    break;
                case TK_BYTES: case TK_STRING: case TK_NAME:
                    printf("[%d] %s: ", ls.linenumber, pylt_lex_get_token_name(ls.token.val));
                    raw_str_print(&ls.token.str);
                    putchar('\n');
                    break;
                case TK_INDENT: case TK_DEDENT:
                    printf("[%d] %s: %d\n", ls.linenumber, pylt_lex_get_token_name(ls.token.val), ls.current_indent);
                    break;
                default:
                    printf("[%d] %s\n", ls.linenumber, pylt_lex_get_token_name(ls.token.val));
            }
        }
        if (ls.token.val == TK_END) break;
    }

    system("pause");
    return 0;
}

#endif
