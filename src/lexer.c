
#include "lexer.h"

const char* pyltL_tokens[] = {
    "<END>", "<ERR>", "<COMMENTS>", "<NEWLINE>", "<INDENT>", "<DEDENT>",
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

    ls->linenumber = 1;
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

int get_token_every_match(LexState *ls, const char *s, int len, ...) {
    va_list args;
    int token = 0;

    va_start(args, len);

    for (int i = 0; i < len; i++) {
        if (s[i] == ls->current) {
            token = va_arg(args, int);
            get_next_ch(ls);
        } else break;
    }

    va_end(args);
    return token;
}

uint32_t get_token_if_match(LexState *ls, const char *s, int len, uint32_t token) {
    uint8_t *pos = fr_savepos(ls->fr);
    uint32_t ch = ls->current;

    for (int i = 0; i < len; i++) {
        if (s[i] == ls->current) {
            get_next_ch(ls);
        } else {
            fr_loadpos(ls->fr, pos);
            ls->current = ch;
            return TK_ERR;
        }
    }

    return token;
}


/* read tokens: / /= // //=
                * *= ** **=
                < <= << <<=
                > >= >> >>= */
uint32_t get_token_1(LexState *ls, uint32_t next1_eq_token, const char *next2_text, uint32_t next2_token1, uint32_t next2_token2) {
    uint32_t default_token = ls->current;
    get_next_ch(ls);
    if (ls->current == '=') {
        get_next_ch(ls);
        return next1_eq_token;
    }
    uint32_t tmp = get_token_every_match(ls, next2_text, 2, next2_token1, next2_token2);
    return tmp ? tmp : default_token;
}

/* read tokens: == += -= %= @= &= |= ^= */
uint32_t get_token_2(LexState *ls, uint32_t next_eq_token) {
    uint32_t default_token = ls->current;
    get_next_ch(ls);
    if (ls->current == '=') {
        get_next_ch(ls);
        return next_eq_token;
    }
    return default_token;
}


void pyltL_next(LexState *ls)
{
    // read indent
    int tmp;
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
                break;
            }
            case '\r': {
                cur_indent = 0;
                ls->linenumber++;
                get_next_ch(ls);
                if (ls->current == '\n') get_next_ch(ls);
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
            return;
        case ' ': case '\t':
            get_next_ch(ls);
            break;
        case '(': case ')': case '[': case ']': case '{': case '}':
        case ',': case ':': case '.': case '~':
            ls->token.val = ls->current;
            get_next_ch(ls);
            return;
        case '<':
            ls->token.val = get_token_1(ls, TK_OP_LE, "<=", TK_OP_LSHIFT, TK_DE_LSHIFT_EQ);
            return;
        case '>':
            ls->token.val = get_token_1(ls, TK_OP_GE, ">=", TK_OP_RSHIFT, TK_DE_RSHIFT_EQ);
            return;
        case '/':
            ls->token.val = get_token_1(ls, TK_DE_DIV_EQ, "/=", TK_OP_FLOORDIV, TK_DE_FLOORDIV_EQ);
            return;
        case '*':
            ls->token.val = get_token_1(ls, TK_DE_MUL_EQ, "*=", TK_OP_POW, TK_DE_POW_EQ);
            return;
        case '=':
            ls->token.val = get_token_2(ls, TK_OP_EQ);
            return;
        case '+':
            ls->token.val = get_token_2(ls, TK_DE_PLUS_EQ);
            return;
        case '-':
            get_next_ch(ls);
            if (ls->current == '=') {
                get_next_ch(ls);
                ls->token.val = TK_DE_MINUS_EQ;
            } else if (ls->current == '>') {
                get_next_ch(ls);
                ls->token.val = TK_DE_RET_TYPE;
            } else ls->token.val = '-';
            return;
        case '%':
            ls->token.val = get_token_2(ls, TK_DE_MOD_EQ);
            return;
        case '@':
            ls->token.val = get_token_2(ls, TK_DE_MATMUL_EQ);
            return;
        case '&':
            ls->token.val = get_token_2(ls, TK_DE_BITAND_EQ);
            return;
        case '|':
            ls->token.val = get_token_2(ls, TK_DE_BITOR_EQ);
            return;
        case '^':
            ls->token.val = get_token_2(ls, TK_DE_BITXOR_EQ);
            return;
        case '!':
            ls->token.val = get_token_if_match(ls, "!=", 2, TK_OP_NE);
            return;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            ls->token.val = TK_INT;
            get_next_ch(ls);
            return;
        default:
            ls->token.val = TK_END;
            return;
        }
    }
}
