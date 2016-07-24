
#include "config.h"
#include "lexer.h"
#include "parser.h"
#include "io.h"
#include "debug.h"
#include "vm.h"
#include "api.h"
#include "intp.h"

#include "types/object.h"


#ifdef SHELL

int main(int argc,char* argv[]) {
    int size;
    char *buf = read_file("test.py", &size);
    if (!buf) return 0;

    platform_init();

    PyLiteInterpreter *I = pylt_intp_new();

    StringStream *ss = ss_new(buf, size);
    //printf(buf);
    putchar('\n');

    /*debug_test_lexer(&I, ss);
    system("pause");
    return 0;*/

    pylt_intp_load_stream(I, ss);
    PyLiteCodeObject *code = kv_top(I->vm.frames).code;

    putchar('\n');
    debug_print_const_vals(I, code);
    debug_print_opcodes(I, code);
    putchar('\n');

    pylt_intp_run(I);
    pylt_intp_finalize(I);

#ifdef PLATFORM_WINDOWS
    system("pause");
#endif
    return 0;
}

#endif
