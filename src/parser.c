
#include "parser.h"
#include "assert.h"

void parse_expr(ParserState *ps);
void parse_expr1(ParserState *ps);
void parse_expr2(ParserState *ps);
void parse_expr3(ParserState *ps);
void parse_expr4(ParserState *ps);
void parse_expr5(ParserState *ps);


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
            next(ps);
            break;
        case TK_BYTES:
            next(ps);
            break;
        default:
            error(ps);
            break;
    }
    raw_str_print(&(tk->str));
    putchar('\n');
}

/*
T ->    ( EXPR ) |
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
        default:
            //print_tk(tk);
            parse_basetype(ps);
    }
}

/* EXPR -> EXPR5 EXPR4 EXPR3 EXPR2 EXPR1 T */
void parse_expr(ParserState *ps) {
    parse_expr5(ps);
    parse_expr4(ps);
    parse_expr3(ps);
    parse_expr2(ps);
    parse_expr1(ps);
}

// 其实还有一个优先级更低的： not
/* EXPR1 -> (>|>=|<|<=|==|!=) EXPR2 | 空 */
void parse_expr1(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '>': case '<': case TK_OP_GE: case TK_OP_LE: case TK_OP_EQ: case TK_OP_NE:
            print_tk(tk);
            next(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            parse_expr2(ps);
            parse_expr1(ps);
            break;
    }
}


/* EXPR2 -> (&|<<|>>||) EXPR3 | 空 */
void parse_expr2(ParserState *ps) {
    //parse_t(ps);
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '&': case '|': case TK_OP_LSHIFT: case TK_OP_RSHIFT:
            print_tk(tk);
            next(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            parse_expr2(ps);
            break;
    }
}

/* EXPR3 -> (+|-) EXPR4 | 空 */
void parse_expr3(ParserState *ps) {
    //parse_t(ps);
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '+': case '-':
            print_tk(tk);
            next(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            break;
    }
}

/* EXPR4 -> (*|/|%|//) EXPR | 空 */
void parse_expr4(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '*': case '/': case '%': case TK_OP_FLOORDIV:
            print_tk(tk);
            next(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            break;
    }
}

/* EXPR5 -> (**) EXPR | 空 */
void parse_expr5(ParserState *ps) {
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
