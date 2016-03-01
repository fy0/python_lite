
#include "lexer.h"
#include "types/string.h"
#include "types/bytes.h"
#include "types/number.h"

uint32_t get_token_1(StringStream *ss, uint32_t next1_eq_token, uint32_t next1_token, uint32_t next2_token);

const char* pylt_lex_tokens[] = {
    "<END>", "<COMMENTS>", "<NEWLINE>", "<INDENT>", "<DEDENT>",
    "<NAME>", "<INT>", "<FLOAT>", "<STRING>", "<BYTES>",

    "False", "None", "True", "and", "as", "assert", "break",
    "class", "continue", "def", "del", "elif", "else", "except",
    "finally", "for", "from", "global", "if", "import", "in",
    "is", "lambda", "nonlocal", "not", "or", "pass", "raise",
    "return", "try", "while", "with", "yield",

    "**", "//", "<<", ">>", "<=", ">=", "==", "!=",

    "->",
    "+=", "-=", "*=", "/=", "//=", "%=", "@=",
    "&=", "|=", "^=", ">>=", "<<=", "**=",
};

const char* pylt_lex_get_token_name(uint32_t token) {
    switch (token) {
        case '+': return "+"; case '-': return "-"; case '*': return "*"; case '/': return "/";
        case '%': return "%"; case '@': return "@"; case '&': return "&"; case '|': return "|";
        case '^': return "^"; case '~': return "~"; case '<': return "<"; case '>': return ">";
        case '(': return "("; case ')': return ")"; case '[': return "["; case ']': return "]";
        case '{': return "{"; case '}': return "}"; case ',': return ","; case ':': return ":";
        case '.': return "."; case '=': return "=";
    }
    if (token >= FIRST_TOKEN)
        return pylt_lex_tokens[token - FIRST_TOKEN];
    return NULL;
}

void pylt_lex_init(PyLiteState* state, LexState *ls, StringStream *ss) {
    IndentInfo *idt;
    ls->state = state;
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

    ls->le.bytes.buf = pylt_realloc(NULL, PYLT_LEX_STR_DEFAULT_BUFFER_SIZE);
    ls->le.bytes.size = PYLT_LEX_STR_DEFAULT_BUFFER_SIZE;

    ls->le.str.buf = pylt_realloc(NULL, PYLT_LEX_BYTES_DEFAULT_BUFFER_SIZE);
    ls->le.str.size = PYLT_LEX_BYTES_DEFAULT_BUFFER_SIZE;
}


/* read tokens: / /= // //=
                * *= ** **=
                < <= << <<=
                > >= >> >>= */
_INLINE
uint32_t get_token_1(StringStream *ss, uint32_t next1_eq_token, uint32_t next1_token, uint32_t next2_token) {
    uint32_t default_token = ss->current;
    ss_nextc(ss);
    if (ss->current == '=') {
        ss_nextc(ss);
        return next1_eq_token;
    } else if (ss->current == default_token) {
        ss_nextc(ss);
        if (ss->current == '=') {
            ss_nextc(ss);
            return next2_token;
        }
        return next1_token;
    }
    return default_token;
}

/* read tokens: == += -= %= @= &= |= ^= */
static _INLINE
uint32_t get_token_2(StringStream *ss, uint32_t next_eq_token) {
    uint32_t default_token = ss->current;
    ss_nextc(ss);
    if (ss->current == '=') {
        ss_nextc(ss);
        return next_eq_token;
    }
    return default_token;
}

uint32_t read_str_or_bytes_head(StringStream *ss, bool *is_raw) {
    StringStreamSave save;
    ss_savepos(ss, &save);
    uint8_t state = 0; // r1 u2 b4
    *is_raw = false;

    for (;;) {
        switch (ss->current) {
        case '\'': case '\"':
            if ((state & 1) && is_raw) *is_raw = true;
            if (state & 4) return TK_BYTES;
            else return TK_STRING;
        case 'r': case 'R':
            if (state & 1) goto _not_str;
            state |= 1;
            ss_nextc(ss);
            break;
        case 'u': case 'U':
            if (state & (2 | 4)) goto _not_str;
            state |= 2;
            ss_nextc(ss);
            break;
        case 'b': case 'B':
            if (state & (2 | 4)) goto _not_str;
            state |= 4;
            ss_nextc(ss);
            break;
        default:
            goto _not_str;
        }
    }

_not_str:
    ss_loadpos(ss, &save);
    return 0;
}

_INLINE static bool bytes_next(LexState *ls, uint32_t chr) {
    if (ls->le.bytes.pos+1 == ls->le.bytes.size) {
        ls->le.bytes.buf = pylt_realloc(ls->le.bytes.buf, ls->le.bytes.size += PYLT_LEX_BYTES_DEFAULT_BUFFER_INC_STEP);
    }
    ls->le.bytes.buf[ls->le.bytes.pos++] = (char)chr;
    return true;
}

_INLINE static bool str_next(LexState *ls, uint32_t chr) {
    if (ls->le.str.pos + 1 == ls->le.str.size) {
        ls->le.str.buf = pylt_realloc(ls->le.str.buf, ls->le.str.size += PYLT_LEX_STR_DEFAULT_BUFFER_INC_STEP);
    }
    ls->le.str.buf[ls->le.str.pos++] = chr;
    return true;
}

_INLINE static bool bs_next(LexState *ls, uint32_t chr, bool is_str) {
    return (is_str) ? str_next(ls, chr) : bytes_next(ls, chr);
}


bool read_str_or_bytes(LexState *ls, bool is_raw) {
    StringStream *ss = ls->ss;
    uint32_t sign = ss->current;
    StringStreamSave save;
    bool is_str_type = ls->token.val == TK_STRING;
    bool is_long_string_or_bytes = false;

    // now : """abc"""
    //       |          <- here

    ss_nextc(ss);
    ss_savepos(ss, &save);

    // long string/bytes check: ''' or """
    if (ss->current == sign) {
        ss_nextc(ss);
        if (ss->current == sign) {
            ss_nextc(ss);
            is_long_string_or_bytes = true;
        }
    }

    // not long string, backtracking
    if (!is_long_string_or_bytes) {
        ss_loadpos(ss, &save);
    }

    if (is_str_type) ls->le.str.pos = 0;
    else ls->le.bytes.pos = 0;

    for (;;) {
        switch (ss->current) {
        case '\'': case '"':
            if (ss->current == sign) {
                if (is_long_string_or_bytes) {
                    ss_nextc(ss);
                    if (ss->current != sign) {
                        bs_next(ls, sign, is_str_type);
                        continue;
                    }
                    ss_nextc(ss);
                    if (ss->current != sign) {
                        bs_next(ls, sign, is_str_type);
                        bs_next(ls, sign, is_str_type);
                        continue;
                    }
                    ss_nextc(ss);
                    goto the_end;
                } else {
                    ss_nextc(ss);
                    goto the_end;
                }
            }
        case '\n': {
            ls->linenumber++;
            if (!is_long_string_or_bytes) return false;
            bs_next(ls, ss->current, is_str_type);
            ss_nextc(ss);
            break;
        }
        case '\r': {
            ls->linenumber++;
            if (!is_long_string_or_bytes) return false;
            bs_next(ls, '\n', is_str_type);
            ss_nextc(ss);
            if (ss->current == '\n') ss_nextc(ss);
            break;
        }
        //case '\\': {} TODO
        default:
            if (ss->current >= 0x80) {
                //SyntaxError: bytes can only contain ASCII literal characters.
                return false;
            }
            bs_next(ls, ss->current, is_str_type);
            ss_nextc(ss);
        }
    }
    return false;

the_end:
    ls->token.obj = (is_str_type) ? castobj(pylt_obj_str_new(ls->state, ls->le.str.buf, ls->le.str.pos, is_raw)) :
                                    castobj(pylt_obj_bytes_new(ls->state, ls->le.bytes.buf, ls->le.bytes.pos, is_raw));
    if (!ls->token.obj) return false;
    return true;
}

uint32_t read_kw_or_id(LexState *ls) {
    const uint32_t *p = ls->le.str.buf;
    int size = ls->le.str.pos;

#define KW_NEXT(c, e) if (c != e) return TK_NAME;

    switch (*p) {
        case 'a': // and as assert
            switch (size) {
                case 2: KW_NEXT(*++p, 's'); return TK_KW_AS;
                case 3: KW_NEXT(*++p, 'n'); KW_NEXT(*++p, 'd'); return TK_KW_AND;
                case 6: KW_NEXT(*++p, 's'); KW_NEXT(*++p, 's'); KW_NEXT(*++p, 'e'); KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 't'); return TK_KW_ASSERT;
                default: return TK_NAME;
            }
        case 'b': // break
            if (size != 5) return TK_NAME;
            KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 'e'); KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'k');
            return TK_KW_BREAK;
        case 'c': // class continue
            if (size == 5) {
                KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 's'); KW_NEXT(*++p, 's');
                return TK_KW_CLASS;
            } else if (size == 8) {
                KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'n'); KW_NEXT(*++p, 't'); KW_NEXT(*++p, 'i'); KW_NEXT(*++p, 'n'); KW_NEXT(*++p, 'u'); KW_NEXT(*++p, 'e');
                return TK_KW_CONTINUE;
            }
            return TK_NAME;
        case 'd': // def del
            if (size == 3) {
                KW_NEXT(*++p, 'e');
                switch (*++p) {
                    case 'f': return TK_KW_DEF;
                    case 'l': return TK_KW_DEL;
                }
            }
            return TK_NAME;
        case 'e': // else elif except
            if (size == 4) {
                KW_NEXT(*++p, 'l');
                switch (*++p) {
                    case 's': KW_NEXT(*++p, 'e'); return TK_KW_ELSE;
                    case 'i': KW_NEXT(*++p, 'f'); return TK_KW_ELIF;
                }
            } else if (size == 6) {
                KW_NEXT(*++p, 'x'); KW_NEXT(*++p, 'c'); KW_NEXT(*++p, 'e'); KW_NEXT(*++p, 'p'); KW_NEXT(*++p, 't');
                return TK_KW_EXCEPT;
            }
            return TK_NAME;
        case 'f': // for from finally
            switch (size) {
                case 3: KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'r'); return TK_KW_FOR;
                case 4: KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'm'); return TK_KW_FROM;
                case 7: KW_NEXT(*++p, 'i'); KW_NEXT(*++p, 'n'); KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'y'); return TK_KW_FINALLY;
                default: return TK_NAME;
            }
        case 'g': // global
            if (size != 6) return TK_NAME;
            KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'b'); KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'l');
            return TK_KW_GLOBAL;
        case 'i': // if in is import
            if (size == 2) {
                switch (*++p) {
                    case 'f': return TK_KW_IF;
                    case 'n': return TK_KW_IN;
                    case 's': return TK_KW_IS;
                }
            } else if (size == 6) {
                KW_NEXT(*++p, 'm'); KW_NEXT(*++p, 'p'); KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 't');
                return TK_KW_IMPORT;
            }
            return TK_NAME;
        case 'l': // lambda
            if (size != 6) return TK_NAME;
            KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'm'); KW_NEXT(*++p, 'b'); KW_NEXT(*++p, 'd'); KW_NEXT(*++p, 'a');
            return TK_KW_LAMBDA;
        case 'n': // not nonlocal
            switch (size) {
                case 3: KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 't'); return TK_KW_NOT;
                case 8: KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'n'); KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'c'); KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'l'); return TK_KW_NONLOCAL;
                default: return TK_NAME;
            }
        case 'o': // or
            if (size != 2) return TK_NAME;
            KW_NEXT(*++p, 'r');
            return TK_KW_OR;
        case 'p': // pass
            if (size != 4) return TK_NAME;
            KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 's'); KW_NEXT(*++p, 's');
            return TK_KW_PASS;
        case 'r': // return raise
            switch (size) {
                case 5: KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'i'); KW_NEXT(*++p, 's'); KW_NEXT(*++p, 'e'); return TK_KW_RAISE;
                case 6: KW_NEXT(*++p, 'e'); KW_NEXT(*++p, 't'); KW_NEXT(*++p, 'u'); KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 'n'); return TK_KW_RETURN;
                default: return TK_NAME;
            }
        case 't': // try
            if (size != 3) return TK_NAME;
            KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 'y');
            return TK_KW_TRY;
        case 'w': // while with
            switch (size) {
                case 4: KW_NEXT(*++p, 'i'); KW_NEXT(*++p, 't'); KW_NEXT(*++p, 'h'); return TK_KW_WITH;
                case 5: KW_NEXT(*++p, 'h'); KW_NEXT(*++p, 'i'); KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'e'); return TK_KW_WHILE;
                default: return TK_NAME;
            }
        case 'y': // yield
            if (size != 5) return TK_NAME;
            KW_NEXT(*++p, 'i'); KW_NEXT(*++p, 'e'); KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 'd');
            return TK_KW_YIELD;
        case 'F': // False
            if (size != 5) return TK_NAME;
            KW_NEXT(*++p, 'a'); KW_NEXT(*++p, 'l'); KW_NEXT(*++p, 's'); KW_NEXT(*++p, 'e');
            return TK_KW_FALSE;
        case 'N': // None
            if (size != 4) return TK_NAME;
            KW_NEXT(*++p, 'o'); KW_NEXT(*++p, 'n'); KW_NEXT(*++p, 'e');
            return TK_KW_NONE;
        case 'T': // True
            if (size != 4) return TK_NAME;
            KW_NEXT(*++p, 'r'); KW_NEXT(*++p, 'u'); KW_NEXT(*++p, 'e');
            return TK_KW_TRUE;
        default:
            return TK_NAME;
    }
}

#define lex_isspace(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == 0)
#define lex_isnumber(c) (c >= '0' && c <= '9')
#define lex_ishex(c) (lex_isnumber(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
#define lex_isbin(c) (c == '0' || c == '1')
#define lex_isoct(c) (c >= '0' && c <= '7')
#define lex_isdec(c) (c >= '0' && c <= '9')
//#define lex_isescapenumflag(c) (c == 'x' || c == 'X' || c == 'b'  || c == 'B' || c == 'o' || c == 'O')
#define lex_isidentfirst(c) ((c >= 'A' && c<= 'Z') || (c >= 'a' && c<= 'z') || (c >= '_'))
#define lex_isidentletter(c) ((c >= 'A' && c<= 'Z') || (c >= 'a' && c<= 'z') || (c >= '0' && c<= '9') || (c == '_'))

int pylt_lex_next(LexState *ls)
{
    // read indent
    int cur_indent = ls->current_indent;
    StringStream *ss = ls->ss;
    int tmp, tmp2;

    if (cur_indent == -1) {
        cur_indent = 0;
        ss_nextc(ss);
        for (;;) {
            switch (ss->current) {
            case '#':
                do { ss_nextc(ss); } 
                while (ss->current != '\n' && ss->current != '\r');
                break;
            case '\n':
                cur_indent = 0;
                ls->linenumber++;
                ss_nextc(ss);
                break;
            case '\r':
                cur_indent = 0;
                ls->linenumber++;
                ss_nextc(ss);
                if (ss->current == '\n') ss_nextc(ss);
                break;
            case ' ':
                cur_indent++;
                ss_nextc(ss);
                break;
            case '\t':
                cur_indent += 8;
                ss_nextc(ss);
                break;
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
            return 0;
        } else {
            IndentInfo *idt = ls->indent;
            ls->indent = ls->indent->prev;
            idt->prev = ls->indent_used;
            ls->indent_used = idt;

            ls->token.val = TK_DEDENT;
            return 0;
        }
    }

    // read tokens
    for (;;) {
        switch (ss->current) {
            case '\n': case '\r':
                ls->current_indent = -1;
                ls->token.val = TK_NEWLINE;
                return 0;
            case ' ': case '\t':
                ss_nextc(ss);
                break;
            case '(': case ')': case '[': case ']': case '{': case '}':
            case ',': case ':': case '.': case '~':
                ls->token.val = ss->current;
                ss_nextc(ss);
                return 0;
            case '<': // < << <= <<=
                ls->token.val = get_token_1(ss, TK_OP_LE, TK_OP_LSHIFT, TK_DE_LSHIFT_EQ);
                return 0;
            case '>': // > >> >= >>=
                ls->token.val = get_token_1(ss, TK_OP_GE, TK_OP_RSHIFT, TK_DE_RSHIFT_EQ);
                return 0;
            case '/': // / // //=
                ls->token.val = get_token_1(ss, TK_DE_DIV_EQ, TK_OP_FLOORDIV, TK_DE_FLOORDIV_EQ);
                return 0;
            case '*': // * ** **=
                ls->token.val = get_token_1(ss, TK_DE_MUL_EQ, TK_OP_POW, TK_DE_POW_EQ);
                return 0;
            case '=': // = ==
                ls->token.val = get_token_2(ss, TK_OP_EQ);
                return 0;
            case '+': // + +=
                ls->token.val = get_token_2(ss, TK_DE_PLUS_EQ);
                return 0;
            case '-': // - -= ->
                ss_nextc(ss);
                switch (ss->current) {
                    case '=': ss_nextc(ss); ls->token.val = TK_DE_MINUS_EQ; break;
                    case '>': ss_nextc(ss); ls->token.val = TK_DE_RET_TYPE; break;
                    default: ls->token.val = '-';
                }
                return 0;
            case '%': // % %=
                ls->token.val = get_token_2(ss, TK_DE_MOD_EQ);
                return 0;
            case '@': // # #=
                ls->token.val = get_token_2(ss, TK_DE_MATMUL_EQ);
                return 0;
            case '&': // & &=
                ls->token.val = get_token_2(ss, TK_DE_BITAND_EQ);
                return 0;
            case '|': // | |=
                ls->token.val = get_token_2(ss, TK_DE_BITOR_EQ);
                return 0;
            case '^': // ^ ^=
                ls->token.val = get_token_2(ss, TK_DE_BITXOR_EQ);
                return 0;
            case '!': // !=
                ss_nextc(ss);
                if (ss->current != '=') return PYLT_ERR_LEX_INVALID_CHARACTER;
                ss_nextc(ss);
                ls->token.val = TK_OP_NE;
                return 0;
            case '0':
                ls->le.bytes.pos = 0;
                bytes_next(ls, '0');
                ss_nextc(ss);
                switch (ss->current) {
                    case 'x': case 'X':
                        ss_nextc(ss);
                        if (!lex_ishex(ss->current)) return PYLT_ERR_LEX_INVALID_NUMBER;
                        tmp = 1; // hex
                        ls->le.bytes.pos = 0;
                        while (lex_ishex(ss->current)) { bytes_next(ls, ss->current); ss_nextc(ss); }
                        break;
                    case 'b': case 'B':
                        ss_nextc(ss);
                        if (!lex_isbin(ss->current)) return PYLT_ERR_LEX_INVALID_NUMBER;
                        tmp = 2; // bin
                        ls->le.bytes.pos = 0;
                        while (lex_isbin(ss->current)) { bytes_next(ls, ss->current); ss_nextc(ss); }
                        break;
                    case 'o': case 'O':
                        ss_nextc(ss);
                        if (!lex_isoct(ss->current)) return PYLT_ERR_LEX_INVALID_NUMBER;
                        tmp = 3; // oct
                        ls->le.bytes.pos = 0;
                        while (lex_isoct(ss->current)) { bytes_next(ls, ss->current); ss_nextc(ss); }
                        break;
                    case '.':
                        goto read_dec_float;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        bytes_next(ls, ss->current);
                        ss_nextc(ss);
                        goto read_dec_float;
                    default:
                        tmp = 0;
                }

                ls->token.val = TK_INT;
                ls->token.obj = castobj(pylt_obj_int_new_from_cstr_full(ls->state, ls->le.bytes.buf, ls->le.bytes.pos, tmp));
                return 0;
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                ls->le.bytes.pos = 0;
                bytes_next(ls, ss->current);
                ss_nextc(ss);
            read_dec_float:
                while (lex_isdec(ss->current)) { bytes_next(ls, ss->current); ss_nextc(ss); }

                if (ss->current == '.') {
                    tmp2 = ls->le.bytes.pos;
                    bytes_next(ls, '.');
                    ss_nextc(ss);
                    while (lex_isdec(ss->current)) { bytes_next(ls, ss->current); ss_nextc(ss); }
                    ls->token.val = TK_FLOAT;
                    ls->token.obj = castobj(pylt_obj_float_new_from_cstr_full(ls->state, ls->le.bytes.buf, ls->le.bytes.pos, tmp2));
                } else {
                    ls->token.val = TK_INT;
                    ls->token.obj = castobj(pylt_obj_int_new_from_cstr_full(ls->state, ls->le.bytes.buf, ls->le.bytes.pos, 0));
                }

                return 0;
            case 'r': case 'R': case 'b': case 'B': case 'u': case 'U': case '\'': case '"': {
                bool is_raw;
                uint32_t tok = read_str_or_bytes_head(ss, &is_raw);
                if (tok) {
                    ls->token.val = tok;
                    if (!read_str_or_bytes(ls, is_raw)) return PYLT_ERR_LEX_INVALID_STR_OR_BYTES;
                } else {
                    if (ss->current != '\'' || ss->current != '\"') {
                        goto read_kw_or_id;
                    }
                }
                return 0;
            }
            case '\0':
                ls->token.val = (ls->token.val == TK_NEWLINE) ? TK_END : TK_NEWLINE;
                return 0;
            default:
                if (lex_isidentfirst(ss->current)) {
                read_kw_or_id:
                    ls->le.str.pos = 0;
                    do { str_next(ls, ss->current); ss_nextc(ss); }
                    while (lex_isidentletter(ss->current));
                    ls->token.val = read_kw_or_id(ls);
                    ls->token.obj = castobj(pylt_obj_str_new(ls->state, ls->le.str.buf, ls->le.str.pos, true));
                    return 0;
                }
                return PYLT_ERR_LEX_INVALID_CHARACTER;
        }
    }
}
