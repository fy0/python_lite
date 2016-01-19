
#include "lexer.h"

void get_next_ch(LexState *ls) {
    ls->current = fr_getc_u8(ls->fr);
}

void pyltL_init(LexState *ls, FileReader *fr)
{
    ls->linenumber = 0;
    ls->fr = fr;
}

void pyltL_next(LexState *ls)
{
    get_next_ch(ls);
    for (;;) {
        switch (ls->current) {
            ;
        }
    }
}
