
#include "lexer.h"
#include "parser.h"
#include "utils/io.h"
#include "debug.h"
#include "vm.h"
#include "api.h"
#include "intp.h"

#include "utils/misc.h"
#include "types/object.h"


#ifdef SHELL

#include "deps/linenoise/linenoise.h"

int main(int argc, char* argv[]) {
    platform_init();

    PyLiteInterpreter *I = pylt_intp_new();
    PyLiteFile *input = pl_io_file_new_ex(I, "test.py", "r", PYLT_IOTE_UTF8);
    if (!input) return 0;
    putwchar('\n');

    /*debug_test_lexer(I, input);
    system("pause");
    return 0;*/

    pylt_intp_loadf(I, input);
    PyLiteCodeObject *code = kv_top(I->vm.frames).code;

    debug_print_const_vals(I, code);
    debug_print_opcodes(I, code);
    putwchar('\n');

    pylt_intp_run(I);
    pylt_intp_finalize(I);

#ifdef PLATFORM_WINDOWS
    system("pause");
#endif
    return 0;
}

#endif
