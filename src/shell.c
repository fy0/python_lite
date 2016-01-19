
#include "utils.h"
#include "lexer.h"
#include "io.h"

#ifdef SHELL

int main(int argc,char* argv[])
{
    int size;
    char *buf = read_file("test.py", &size);

    FileReader *fr = fr_init(buf, size);

    LexState ls;
    pyltL_init(&ls, fr);
    pyltL_next(&ls);

    system("pause");
    return 0;
}

#endif
