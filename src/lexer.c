
#include "lexer.h"

const char* pyltL_tokens[] = {
    "<END>", "<COMMENTS>", "<NEWLINE>", "<INDENT>", "<DEDENT>",
    "<NAME>", "<INT>", "<FLOAT>", "<STRING>", "<BYTES>",

    "False", "class", "finally", "is", "return",
    "None", "continue", "for", "lambda", "try",
    "True", "def", "from", "nonlocal", "while",
    "and", "del", "global", "not", "with",
    "as", "elif", "if", "or", "yield",
    "assert", "else", "import", "pass",
    "break", "except", "in", "raise",

    "**", "//", "<<", ">>", "<=", ">=", "==", "!=",

    "->",
    "+=", "-=", "*=", "/=", "//=", "%=", "@=",
    "&=", "|=", "^=", ">>=", "<<=", "**=",
};

const char* pyltL_get_token_name(uint32_t token) {
    if (token >= FIRST_TOKEN)
        return pyltL_tokens[token - FIRST_TOKEN];
    return NULL;
}

void get_next_ch(LexState *ls) {
    ls->current = fr_getc_u8(ls->fr);
}

void pyltL_init(LexState *ls, FileReader *fr)
{
    IndentInfo *idt;

    ls->linenumber = 0;
    ls->fr = fr;
    ls->current_indent = -1;
    
    ls->indent = idt = pylt_realloc(NULL, sizeof(IndentInfo));
    idt->val = 0;
    idt->prev = NULL;

    ls->indent_used = idt = pylt_realloc(NULL, sizeof(IndentInfo));
    for (int i = 0; i < 4; i++) {
        idt->prev = pylt_realloc(NULL, sizeof(IndentInfo));
        idt = idt->prev;
    }
    idt->prev = NULL;
}


void pyltL_next(LexState *ls)
{
    // read indent
    int cur_indent = ls->current_indent;
    if (cur_indent == -1) {
        cur_indent = 0;
        get_next_ch(ls);
        for (;;) {
            switch (ls->current) {
            case '#':
                do { get_next_ch(ls); } 
                while (ls->current != '\n' && ls->current != '\r');
                break;
            case '\n': {
                cur_indent = 0;
                ls->linenumber++;
                get_next_ch(ls);
                if (ls->current == '\r') get_next_ch(ls);
                break;
            }
            case '\r': {
                cur_indent = 0;
                ls->linenumber++;
                get_next_ch(ls);
                break;
            }
            case ' ': {
                cur_indent++;
                get_next_ch(ls);
                break;
            }
            case '\t': {
                cur_indent += 8;
                get_next_ch(ls);
                break;
            }
            default:
                ls->current_indent = cur_indent;
                goto indent_end;
                break;
            }
        }
    }
indent_end:
    if (cur_indent != -1 && cur_indent != ls->indent->val) {
        if (cur_indent > ls->indent->val) {
            IndentInfo *idt;
            if (ls->indent_used) {
                idt = ls->indent_used;
                ls->indent_used = idt->prev;
            } else {
                idt = pylt_realloc(NULL, sizeof(IndentInfo));
            }
            idt->prev = ls->indent;
            idt->val = cur_indent;
            ls->indent = idt;

            ls->token.val = TK_INDENT;
            return;
        } else {
            IndentInfo *idt = ls->indent;
            ls->indent = ls->indent->prev;
            idt->prev = ls->indent_used;
            ls->indent_used = idt;

            ls->token.val = TK_DEDENT;
            return;
        }
    }

    // read tokens
    for (;;) {
        switch (ls->current) {
        case '\n': case '\r':
            ls->current_indent = -1;
            ls->token.val = TK_NEWLINE;
            get_next_ch(ls);
            return;
        case ' ': case '\t':
            get_next_ch(ls);
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            ls->token.val = TK_INT;
            get_next_ch(ls);
            return;
        }
    }
}
