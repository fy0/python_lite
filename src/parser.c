
#include "state.h"
#include "parser.h"
#include "assert.h"
#include "vm.h"
#include "types/bool.h"

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

PyLiteObject* new_obj_number_from_token(PyLiteState *state, Token *tk);


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

void print_tk_val(int tk_val) {
    const char *name = pylt_lex_get_token_name(tk_val);
    if (name) printf("%s", name);
    else putchar(tk_val);
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
        case TK_KW_TRUE:
            kv_pushobj(ps->const_val, castobj(pylt_obj_bool_new(true)));
            next(ps);
            break;
        case TK_KW_FALSE: 
            kv_pushobj(ps->const_val, castobj(pylt_obj_bool_new(false)));
            next(ps);
            break;
        case TK_INT: case TK_FLOAT:
            raw_str_print(&(tk->str));
            kv_pushobj(ps->const_val, castobj(new_obj_number_from_token(ps->state, tk)));
            next(ps);
            break;
        case TK_BYTES:
            next(ps);
            break;
        default:
            error(ps);
            return;
    }
    kv_pushbc(ps->opcodes, BC_LOADCONST);
    kv_pushbc(ps->opcodes, kv_size(ps->const_val));
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
            next(ps);
            parse_expr(ps);
            ACCEPT(ps, ')');
            break;
        case TK_KW_NOT:
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            parse_expr3(ps);
            print_tk_val(TK_KW_NOT);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, token_to_op_val(TK_KW_NOT));
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
            print_tk_val(TK_KW_OR);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, OP_OR);
            break;
    }
}

/* AND EXPR10 ... EXPR2 | ε */
_INLINE void parse_expr2(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_KW_AND:
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
            print_tk_val(TK_KW_AND);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, OP_AND);
            break;
    }
}


/* (<|<=|>|>=|!=|==|in|not in|is|is not) EXPR10 ... EXPR3 | ε */
_INLINE void parse_expr3(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val, op_val;
    switch (tk->val) {
        case '<': case TK_OP_LE: case '>': case TK_OP_GE: case TK_OP_NE: case TK_OP_EQ: case TK_KW_IN:
            tk_val = tk->val;
            op_val = token_to_op_val(tk->val);
            next(ps);
            goto success;
        case TK_KW_IS:
            next(ps);
            if (tk->val == TK_KW_NOT) {
                op_val = OP_IS_NOT;
                next(ps);
            } else op_val = OP_IS;
            goto success;
        case TK_KW_NOT:
            next(ps);
            ACCEPT(ps, TK_KW_IN);
            op_val = OP_NOT_IN;
            goto success;
    }
    return;
success:
    parse_expr10(ps);
    parse_expr9(ps);
    parse_expr8(ps);
    parse_expr7(ps);
    parse_expr6(ps);
    parse_expr5(ps);
    parse_expr4(ps);
    parse_expr3(ps);
    printf("%s\n", pylt_vm_get_op_name(op_val));
    kv_pushbc(ps->opcodes, BC_OPERATOR);
    kv_pushbc(ps->opcodes, op_val);
}


/* | EXPR10 ... EXPR4 | ε */
_INLINE void parse_expr4(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '|':
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            parse_expr4(ps);
            print_tk_val('|');
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, OP_BITOR);
            break;
    }
}


/* ^ EXPR10 ... EXPR5 | ε */
_INLINE void parse_expr5(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '^':
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            parse_expr5(ps);
            print_tk_val('^');
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, OP_BITXOR);
            break;
    }
}



/* & EXPR10 ... EXPR6 | ε */
_INLINE void parse_expr6(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case '&':
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            parse_expr6(ps);
            print_tk_val('&');
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, OP_BITAND);
            break;
    }
}


/* (<<|>>) EXPR10 ... EXPR7 | ε */
_INLINE void parse_expr7(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val;
    switch (tk->val) {
        case TK_OP_LSHIFT: case TK_OP_RSHIFT:
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            parse_expr7(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, token_to_op_val(tk_val));
            break;
    }
}


/* (+|-) EXPR10 ... EXPR8 | ε */
_INLINE void parse_expr8(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val;
    switch (tk->val) {
        case '+': case '-':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            parse_expr8(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, token_to_op_val(tk_val));
            break;
    }
}


/* (*|@|/|//|%) EXPR10 ... EXPR9 | ε */
_INLINE void parse_expr9(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val;
    switch (tk->val) {
        case '*': case '@': case '/': case TK_OP_FLOORDIV: case '%':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            parse_expr9(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, token_to_op_val(tk_val));
            break;
    }
}

/* T (** T | ε) */
_INLINE void parse_expr10(ParserState *ps) {
    parse_t(ps);
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_OP_POW:
            next(ps);
            parse_t(ps);
            print_tk_val(TK_OP_POW);
            kv_pushbc(ps->opcodes, BC_OPERATOR);
            kv_pushbc(ps->opcodes, OP_POW);
            break;
    }
}

void parse(ParserState *ps) {
    next(ps);
    parse_expr(ps);
    kv_pushbc(ps->opcodes, BC_PRINT);
}

void pylt_parser_init(ParserState *ps, PyLiteState* state, LexState *ls) {
    ps->state = state;
    ps->ls = ls;
    kv_init(ps->const_val);
    kv_init(ps->opcodes);
}
