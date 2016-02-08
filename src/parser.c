
#include "parser.h"
#include "assert.h"

void parse_expr(ParserState *ps);
void parse_expr1(ParserState *ps);
void parse_expr2(ParserState *ps);
void parse_expr3(ParserState *ps);
void parse_expr4(ParserState *ps);
void parse_expr5(ParserState *ps);
void parse_expr6(ParserState *ps);
void parse_expr7(ParserState *ps);
void parse_expr8(ParserState *ps);
void parse_expr9(ParserState *ps);
void parse_expr10(ParserState *ps);

void next(ParserState *ps) {
    pylt_lex_next(ps->ls);
}

void error(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    printf("ERROR %d\n", ps->ls->linenumber);
    //if (tk->val == TK_INT) raw_str_print(&(tk->str));
    const char *name = pylt_lex_get_token_name(tk->val);
    if (name) printf("%s", name);
    else putchar(tk->val);
    putchar('\n');
    system("pause");
    exit(-1);
}

void print_tk(Token *tk) {
    const char *name = pylt_lex_get_token_name(tk->val);
    if (name) printf("%s", name);
    else putchar(tk->val);
    putchar('\n');
}

static _INLINE
void ACCEPT(ParserState *ps, int token) {
    if (ps->ls->token.val != token) error(ps);
    next(ps);
}

void parse_number(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    if (tk->val == '+' || tk->val == '-') {
        next(ps); 
    }
}

void parse_op(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '+': case '-': case '*': case '/': case '%': case '@': case '>': case '<':
        case TK_OP_FLOORDIV: case TK_OP_POW: case TK_OP_GE: case TK_OP_LE: case TK_OP_EQ :
            next(ps);
    }
}

void parse_basetype(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_TRUE: case TK_KW_FALSE: case TK_INT: case TK_FLOAT:
            raw_str_print(&(tk->str));
            next(ps);
            break;
        case TK_BYTES:
            next(ps);
            break;
        default:
            error(ps);
            break;
    }
    putchar('\n');
}

/*
T ->    ( EXPR ) |
        not EXPR |
        BASETYPE |
        IDENT
*/
void parse_t(ParserState *ps) { 
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_NAME:
            next(ps);
            break;
        case '(':
            print_tk(tk);
            next(ps);
            parse_expr(ps);
            ACCEPT(ps, ')');
            printf(")\n");
            break;
        case TK_KW_NOT:
            print_tk(tk);
            next(ps);
            parse_expr(ps);
            break;
        default:
            parse_basetype(ps);
    }
}

/* EXPR -> EXPR10 ... EXPR1 */
void parse_expr(ParserState *ps) {
    parse_expr10(ps);
    parse_expr9(ps);
    parse_expr8(ps);
    parse_expr7(ps);
    parse_expr6(ps);
    parse_expr5(ps);
    parse_expr4(ps);
    parse_expr3(ps);
    parse_expr2(ps);
    parse_expr1(ps);
}

/* OR EXPR10 ... EXPR1 | ε */
_INLINE void parse_expr1(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_OR:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            parse_expr2(ps);
            parse_expr1(ps);
            break;
    }
}

/* AND EXPR10 ... EXPR2 | ε */
_INLINE void parse_expr2(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_AND:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            parse_expr2(ps);
            break;
    }
}


/* (<|<=|>|>=|!=|==|in|not in|is|is not) EXPR10 ... EXPR3 | ε */
_INLINE void parse_expr3(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '<': case TK_OP_LE: case '>': case TK_OP_GE: case TK_OP_NE: case TK_OP_EQ: case TK_KW_IS: case TK_KW_IN:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            break;
    }
}


/* | EXPR10 ... EXPR4 | ε */
_INLINE void parse_expr4(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_AND:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            break;
    }
}


/* ^ EXPR10 ... EXPR5 | ε */
_INLINE void parse_expr5(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_AND:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            break;
    }
}



/* & EXPR10 ... EXPR6 | ε */
_INLINE void parse_expr6(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_AND:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            break;
    }
}


/* (<<|>>) EXPR10 ... EXPR7 | ε */
_INLINE void parse_expr7(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_OP_LSHIFT: case TK_OP_RSHIFT:
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            break;
    }
}


/* (+|-) EXPR10 ... EXPR8 | ε */
_INLINE void parse_expr8(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '+': case '-':
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            break;
    }
}


/* (*|@|/|//|%) EXPR10 ... EXPR9 | ε */
_INLINE void parse_expr9(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '*': case '@': case '/': case TK_OP_FLOORDIV: case '%':
            print_tk(tk);
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            break;
    }
}

/* T (** T | ε) */
_INLINE void parse_expr10(ParserState *ps) {
    parse_t(ps);
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_OP_POW:
            print_tk(tk);
            next(ps);
            parse_t(ps);
            break;
    }
}

void parse(ParserState *ps) {
    next(ps);
    parse_expr(ps);
}
