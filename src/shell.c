
#include "utils.h"
#include "lexer.h"
#include "io.h"

#ifdef SHELL

int main(int argc,char* argv[])
{
    int size;
    char *buf = read_file("test.py", &size);

    FileReader *fr = fr_init(buf, size);
    printf(buf);
    putchar('\n');

    LexState ls;
    pyltL_init(&ls, fr);

    for (;;) {
        pyltL_next(&ls);
        if (ls.token.val < FIRST_TOKEN) printf("[%d] %c\n", ls.linenumber, ls.token.val);
        else printf("[%d] %s\n", ls.linenumber, pyltL_get_token_name(ls.token.val));
        if (ls.token.val == TK_END) break;
    }

    system("pause");
    return 0;
}

#endif
