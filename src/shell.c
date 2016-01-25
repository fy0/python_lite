
#include "utils.h"
#include "lexer.h"
#include "io.h"
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
    pyltL_init(&ls, ss);

    for (;;) {
        pyltL_next(&ls);
        if (ls.token.val < FIRST_TOKEN) printf("[%d] %c\n", ls.linenumber, ls.token.val);
        else {
            switch (ls.token.val) {
            case TK_INT:
                printf("[%d] %s: %d\n", ls.linenumber, pyltL_get_token_name(ls.token.val), ls.token.info.i32);
                break;
            case TK_FLOAT:
                printf("[%d] %s: %f\n", ls.linenumber, pyltL_get_token_name(ls.token.val), ls.token.info.f64);
                break;
            default:
                printf("[%d] %s\n", ls.linenumber, pyltL_get_token_name(ls.token.val));
            }
        }
        if (ls.token.val == TK_END) break;
    }

    system("pause");
    return 0;
}

#endif
