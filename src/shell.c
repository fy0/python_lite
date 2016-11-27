
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

#include "deps/linenoise/linenoise.h"

void completion(const wchar_t *buf, linenoiseCompletions *lc) {
    if (buf[0] == L'h') {
        linenoiseAddCompletion(lc, L"hello");
        linenoiseAddCompletion(lc, L"hello there");
        linenoiseAddCompletion(lc, L"测试3");
    }
}

int main(int argc, char* argv[]) {
    platform_init();
    wchar_t *line;
    char *prgname = argv[0];

    /* Parse options, with --multiline we enable multi line editing. */
    while (argc > 1) {
        argc--;
        argv++;
        if (!strcmp(*argv, "--multiline")) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
        } else if (!strcmp(*argv, "--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else {
            fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
            exit(1);
        }
    }

    /* Set the completion callback. This will be called every time the
    * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);

    /* Load history from file. The history file is just a plain text file
    * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /* Now this is the main loop of the typical linenoise-based application.
    * The call to linenoise() will block as long as the user types something
    * and presses enter.
    *
    * The typed string is returned as a malloc() allocated string by
    * linenoise, so the user needs to free() it. */
    while ((line = linenoise(L"a啊测试> ")) != NULL) {
        /* Do something with the string. */
        if (line[0] != '\0' && line[0] != '/') {
            wprintf(L"echo: '%ls'\n", line);
            linenoiseHistoryAdd(line); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
        } else if (!wcsncmp(line, L"/historylen", 11)) {
            /* The "/historylen" command will change the history len. */
            int len = _wtoi(line + 11);
            linenoiseHistorySetMaxLen(len);
        } else if (line[0] == '/') {
            printf_u8("Unreconized command: %s\n", line);
        }
        free(line);
    }
    return 0;

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
