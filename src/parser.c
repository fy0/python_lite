
#include "state.h"
#include "parser.h"
#include "assert.h"
#include "vm.h"
#include "types/bool.h"

void func_save(ParserState *ps);
void func_pop(ParserState *ps);

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
PyLiteObject* new_obj_bytes_from_token(PyLiteState *state, Token *tk);


void next(ParserState *ps) {
    pylt_lex_next(ps->ls);
}

void error(ParserState *ps, int code) {
    Token *tk = &(ps->ls->token);
    printf("ERROR %d\n", ps->ls->linenumber);
    //if (tk->val == TK_INT) raw_str_print(&(tk->str));
    const char *name = pylt_lex_get_token_name(tk->val);
    if (name) printf("%s", name);
    else putchar(tk->val);
    putchar('\n');
    switch (code) {
        case PYLT_ERR_PARSER_INVALID_SYNTAX:
            printf("SyntaxError: invalid syntax\n");
            break;
        case PYLT_ERR_PARSER_BYTES_INVALID_ESCAPE:
            printf("SyntaxError: (value error) invalid escape\n");
            break;
    }
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
    if (ps->ls->token.val != token) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
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
    PyLiteObject *obj;

    switch (tk->val) {
        case TK_KW_TRUE:
            kv_pushobj(ps->func->const_val, castobj(pylt_obj_bool_new(ps->state, true)));
            next(ps);
            break;
        case TK_KW_FALSE: 
            kv_pushobj(ps->func->const_val, castobj(pylt_obj_bool_new(ps->state, false)));
            next(ps);
            break;
        case TK_INT: case TK_FLOAT:
            raw_str_print(&(tk->str));
            kv_pushobj(ps->func->const_val, castobj(new_obj_number_from_token(ps->state, tk)));
            next(ps);
            break;
        case TK_BYTES:
            raw_str_print(&(tk->str));
            obj = castobj(new_obj_bytes_from_token(ps->state, tk));
            if (!obj) {
                error(ps, PYLT_ERR_PARSER_BYTES_INVALID_ESCAPE);
                return;
            }
            kv_pushobj(ps->func->const_val, obj);
            next(ps);
            break;
        default:
            error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            return;
    }
    kv_pushbc(ps->func->opcodes, BC_LOADCONST);
    kv_pushbc(ps->func->opcodes, kv_size(ps->func->const_val));
    putchar('\n');
}

/*
T ->    ( EXPR ) |
        not EXPR |
        BASETYPE |
        IDENT
*/
void parse_t(ParserState *ps) {
    int tk_val;
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
        case '+': case '-':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            print_tk_val(tk_val);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, tk_val == '+' ? OP_POS : OP_NEG);
            break;
        case '~':
            next(ps);
            parse_expr10(ps);
            print_tk_val('~');
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, token_to_op_val('~'));
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
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, token_to_op_val(TK_KW_NOT));
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
            print_tk_val(TK_KW_OR);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, OP_OR);
            parse_expr1(ps);
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
            print_tk_val(TK_KW_AND);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, OP_AND);
            parse_expr2(ps);
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
    printf("%s\n", pylt_vm_get_op_name(op_val));
    kv_pushbc(ps->func->opcodes, BC_OPERATOR);
    kv_pushbc(ps->func->opcodes, op_val);
    parse_expr3(ps);
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
            print_tk_val('|');
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, OP_BITOR);
            parse_expr4(ps);
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
            print_tk_val('^');
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, OP_BITXOR);
            parse_expr5(ps);
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
            print_tk_val('&');
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, OP_BITAND);
            parse_expr6(ps);
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
            print_tk_val(tk_val);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, token_to_op_val(tk_val));
            parse_expr7(ps);
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
            print_tk_val(tk_val);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, token_to_op_val(tk_val));
            parse_expr8(ps);
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
            print_tk_val(tk_val);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, token_to_op_val(tk_val));
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
            next(ps);
            parse_t(ps);
            print_tk_val(TK_OP_POW);
            kv_pushbc(ps->func->opcodes, BC_OPERATOR);
            kv_pushbc(ps->func->opcodes, OP_POW);
            break;
    }
}

void parse(ParserState *ps) {
    next(ps);
    parse_expr(ps);
    kv_pushbc(ps->func->opcodes, BC_PRINT);
}

void func_save(ParserState *ps) {
    kv_push(PyLiteFunctionObject*, ps->func_stack, ps->func);
    ps->func = pylt_obj_func_new(ps->state);
}

void func_pop(ParserState *ps) {
    ps->func = kv_pop(ps->func_stack);
}

void pylt_parser_init(ParserState *ps, PyLiteState* state, LexState *ls) {
    ps->state = state;
    ps->ls = ls;
    ps->func = pylt_obj_func_new(ps->state);
    state->func = ps->func;
    kv_init(ps->func_stack);
}
