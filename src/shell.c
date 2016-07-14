
#include "config.h"
#include "lexer.h"
#include "parser.h"
#include "io.h"
#include "debug.h"
#include "vm.h"
#include "api.h"
#include "state.h"

#include "types/object.h"


#ifdef SHELL

int main(int argc,char* argv[])
{
    int size;
    char *buf = read_file("test.py", &size);
    if (!buf) return 0;

    platform_init();

    PyLiteState state;
    pylt_state_init(&state);

    StringStream *ss = ss_new(buf, size);
    //printf(buf);
    putchar('\n');

    /*debug_test_lexer(&state, ss);
    system("pause");
    return 0;*/

    pylt_state_load_stream(&state, ss);
    PyLiteCodeObject *code = kv_top(state.vm.frames).code;

    putchar('\n');
    debug_print_const_vals(&state, code);
    debug_print_opcodes(&state, code);
    putchar('\n');

    pylt_state_run(&state);
    pylt_state_finalize(&state);

#ifdef PLATFORM_WINDOWS
    system("pause");
#endif
    return 0;
}

#endif
