
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
    ls->current = ss_nextc(ls->ss);
}

void pyltL_init(LexState *ls, StringStream *ss)
{
    IndentInfo *idt;

    ls->linenumber = 1;
    ls->ss = ss;
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
    StringStreamSave save;
    ss_savepos(ls->ss, &save, ls->current);

    for (int i = 0; i < len; i++) {
        if (s[i] == ls->current) {
            get_next_ch(ls);
        } else {
            ls->current = ss_loadpos(ls->ss, &save);
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

_INLINE static
uint8_t _hex(uint32_t code)
{
    if (code >= '0' && code <= '9')
        return code - '0';
    else if (code >= 'A' && code <= 'F')
        return code - 'A' + 10;
    else if (code >= 'a' && code <= 'f')
        return code - 'a' + 10;
    return 255;
}

_INLINE static
uint8_t _oct(uint32_t code)
{
    if (code >= '0' && code <= '7')
        return code - '0';
    return 255;
}

_INLINE static
uint8_t _bin(uint32_t code)
{
    if (code >= '0' && code <= '1')
        return code - '0';
    return 255;
}

_INLINE static
uint8_t _dec(uint32_t code)
{
    if (code >= '0' && code <= '9')
        return code - '0';
    return 255;
}

int _read_x_int(LexState *ls, int n, uint8_t(*func)(uint32_t code)) {
    int ret = 0;
    int pos = -1, val, i;
    uint8_t tmp;
    uint8_t num[30];

    while (true) {
        tmp = (*func)(ls->current);
        if (tmp == 255) break;
        num[++pos] = tmp;
        get_next_ch(ls);
    }

    if (pos == -1) {
        return -1;
    }

    i = 0;
    val = (int)pow(n, pos);
    do {
        ret += num[i++] * val;
        val /= n;
    } while (pos-- > 0);

    return ret;
}

double _read_x_float(LexState *ls, uint8_t(*func)(uint32_t code)) {
    int ret = 0;
    int pos = -1, val, i, max;
    uint8_t tmp;
    uint8_t num[30];

    while (true) {
        tmp = (*func)(ls->current);
        if (tmp == 255) break;
        num[++pos] = tmp;
        get_next_ch(ls);
    }

    if (pos == -1) {
        return -1;
    }

    i = 0;
    max = val = (int)pow(10, pos);
    do {
        ret += num[i++] * val;
        val /= 10;
    } while (pos-- > 0);

    return ret / (double)(max * 10);
}

void read_escape_number(LexState *ls) {
    int tmp;

    switch (ls->current) {
    case 'x': case 'X':
        get_next_ch(ls);
        tmp = _read_x_int(ls, 16, &_hex);
        if (tmp == -1) ls->token.val = TK_ERR;
        else {
            ls->token.val = TK_INT;
            ls->token.info.i32 = tmp;
        }
        break;
    case 'b': case 'B':
        get_next_ch(ls);
        tmp = _read_x_int(ls, 2, &_bin);
        if (tmp == -1) ls->token.val = TK_ERR;
        else {
            ls->token.val = TK_INT;
            ls->token.info.i32 = tmp;
        }
        break;
    case 'o': case 'O':
        get_next_ch(ls);
        tmp = _read_x_int(ls, 8, &_oct);
        if (tmp == -1) ls->token.val = TK_ERR;
        else {
            ls->token.val = TK_INT;
            ls->token.info.i32 = tmp;
        }
        break;
    default:
        ls->token.val = TK_ERR;
        break;
    }
}

void read_number(LexState *ls){
    int a = _read_x_int(ls, 10, &_dec);
    double b;

    if (a != -1) {
        if (ls->current == '.') {
            get_next_ch(ls);
            b = _read_x_float(ls, &_dec);
            if (b == -1) {
                ls->token.val = TK_ERR;
                return;
            }
            ls->token.val = TK_FLOAT;
            ls->token.info.f64 = a + b;
            return;
        } else {
            ls->token.val = TK_INT;
            ls->token.info.i32 = (int)a;
            return;
        }
    }

    ls->token.val = TK_ERR;
    return;
}

#define lex_isspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == 0)

uint32_t read_str_or_bytes_head(LexState *ls, bool *is_raw) {
    StringStreamSave save;
    ss_savepos(ls->ss, &save, ls->current);
    uint8_t state = 0; // r1 u2 b4

    for (;;) {
        switch (ls->current) {
        case '\'': case '\"':
            if ((state & 1) && is_raw) *is_raw = true;
            if (state & 4) return TK_BYTES;
            else return TK_STRING;
        case 'r': case 'R':
            if (state & 1) goto _not_str;
            state |= 1;
            get_next_ch(ls);
            break;
        case 'u': case 'U':
            if (state & (2 | 4)) goto _not_str;
            state |= 2;
            get_next_ch(ls);
            break;
        case 'b': case 'B':
            if (state & (2 | 4)) goto _not_str;
            state |= 4;
            get_next_ch(ls);
            break;
        }
    }

_not_str:
    ls->current = ss_loadpos(ls->ss, &save);
    return 0;
}

bool read_bytes(LexState *ls, bool is_raw) {
    StringStreamSave save;
    uint32_t sign = ls->current;
    bool is_long_string_or_bytes = false;

    get_next_ch(ls);
    ss_savepos(ls->ss, &save, ls->current);

    // long string/bytes check: ''' or """
    if (ls->current == sign) {
        get_next_ch(ls);
        if (ls->current == sign) {
            get_next_ch(ls);
            is_long_string_or_bytes = true;
        }
    }

    if (!is_long_string_or_bytes) {
        ls->current = ss_loadpos(ls->ss, &save);
    }

    for (;;) {
        switch (ls->current) {
        case '\'': case '"':
            if (ls->current == sign) {
                if (is_long_string_or_bytes);
                else {
                    ls->token.val = TK_BYTES;
                    return true;
                }
            }
        case '\n': {
            ls->linenumber++;
            if (!is_long_string_or_bytes) ; // err
        }
        case '\r': {
            ls->linenumber++;
            if (!is_long_string_or_bytes); // err
            break;
        }
        default: {
            ;
        }
        }
    }
}

bool read_string(LexState *ls, bool is_raw) {
    ;
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
        case '0':
            get_next_ch(ls);
            if (lex_isspace(ls->current)) {
                ls->token.val = TK_INT;
                ls->token.info.i32 = 0;
            } else {
                read_escape_number(ls);
                return;
            }
            break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            read_number(ls);
            return;
        case 'r': case 'R': case 'b': case 'B': case 'u': case 'U': case '\'': case '"': {
            bool is_raw;
            uint32_t tok = read_str_or_bytes_head(ls, &is_raw);
            if (!tok) {
                if (tok == TK_BYTES) read_bytes(ls, is_raw);
                else read_string(ls, is_raw);
                break;
            }
        }
        default:
            ls->token.val = TK_END;
            return;
        }
    }
}
