
#include "state.h"
#include "parser.h"
#include "debug.h"
#include "vm.h"
#include "api.h"
#include "bind.h"
#include "types/all.h"

void func_push(ParserState *ps);
ParserInfo* func_pop(ParserState *ps);

void parse_t(ParserState *ps);

void parse_stmts(ParserState *ps);
void parse_block(ParserState *ps);

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

bool parse_try_t(ParserState *ps);
bool parse_try_expr(ParserState *ps);
bool parse_try_expr10(ParserState *ps);

void parse_func(ParserState *ps);
void parse_class(ParserState *ps);
void parse_value_assign(ParserState *ps);
void parse_inplace_op(ParserState *ps, int op_val);


void next(ParserState *ps) {
    pylt_lex_next(ps->ls);
}

/**
1b        1b         2b
opcode    extarg     extra
*/
PyLiteInstruction build_ins(uint8_t opcode, uint8_t exarg, int16_t extra) {
    PyLiteInstruction ret = {
        .code = opcode,
        .exarg = exarg,
        .extra = extra
    };
    return ret;
}

/**
1b        1b         2b
opcode    extarg     extra
*/
void write_ins(ParserState *ps, uint8_t opcode, uint8_t exarg, int16_t extra) {
    PyLiteInstruction ins = build_ins(opcode, exarg, extra);
    kv_pushins(ps->info->code->opcodes, ins);
}

int store_const(ParserState *ps, PyLiteObject *obj) {
    PyLiteListObject *const_val = ps->info->code->const_val;
    pl_int_t index = pylt_obj_list_index_strict(ps->state, const_val, obj);
    if (index == -1) index = pylt_obj_list_append(ps->state, const_val, obj);
    return index;
}

void sload_const(ParserState *ps, PyLiteObject *obj) {
    write_ins(ps, BC_LOADCONST, 0, store_const(ps, obj));
}

void error(ParserState *ps, int code) {
    Token *tk = &(ps->ls->token);
    printf("ERROR at line [%d]\n", ps->ls->linenumber);
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
        case PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP:
            printf("SyntaxError: 'break' outside loop\n");
            break;
        case PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP:
            printf("SyntaxError: 'continue' not properly in loop\n");
            break;
        case PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL:
            printf("SyntaxError: can't assign to literal\n");
            break;
        case PYLT_ERR_PARSER_RETURN_OUTSIDE_FUNCTION:
            printf("SyntaxError: 'return' outside function\n");
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

PyLiteObject* parse_get_consttype(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;

    switch (tk->val) {
        case TK_KW_TRUE:
            next(ps);
            return castobj(pylt_obj_bool_new(ps->state, true));
        case TK_KW_FALSE:
            next(ps);
            return castobj(pylt_obj_bool_new(ps->state, false));
        case TK_INT: case TK_FLOAT: case TK_STRING: case TK_BYTES:
            obj = tk->obj;
            next(ps);
            return obj;
        default:
            return NULL;
    }
}

int parse_mutabletype(ParserState *ps, int *ptimes) {
    Token *tk = &(ps->ls->token);
    int tmp = 0;
    bool old_disable_expr_tuple_parse;

    switch (tk->val) {
        case '[':
            next(ps);
            if (tk->val == ']') {
                next(ps);
                if (ptimes) *ptimes = 0;
                return PYLT_OBJ_TYPE_LIST;
            } else {
                tmp = 0;
                old_disable_expr_tuple_parse = ps->disable_expr_tuple_parse;
                ps->disable_expr_tuple_parse = true;
                while (true) {
                    if (tk->val == ']') break;
                    parse_expr(ps);
                    tmp++;
                    if (tk->val == ',') next(ps);
                    else if (tk->val == ']') break;
                    else return 0;
                }
                next(ps);
                if (ptimes) *ptimes = tmp;
                ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
                return PYLT_OBJ_TYPE_LIST;
            }
            break;
        case '{':
            next(ps);
            if (tk->val == '}') {
                if (ptimes) *ptimes = 0;
                next(ps);
                return PYLT_OBJ_TYPE_DICT;
            }

            old_disable_expr_tuple_parse = ps->disable_expr_tuple_parse;
            ps->disable_expr_tuple_parse = true;
            parse_expr(ps);
            tmp = 1;
            switch (tk->val) {
                case ',': // set
                    next(ps);
                    while (true) {
                        if (!parse_try_expr(ps)) break;
                        tmp++;
                        if (tk->val != ',') break;
                        else next(ps);
                    }
                case '}':
                    ACCEPT(ps, '}');
                    if (ptimes) *ptimes = tmp;
                    ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
                    return PYLT_OBJ_TYPE_SET;
                case ':': // dict
                    next(ps);
                    parse_expr(ps);
                    tmp = 1;
                    while (true) {
                        if (tk->val != ',') break;
                        next(ps);
                        if (!parse_try_expr(ps)) break;
                        ACCEPT(ps, ':');
                        parse_expr(ps);
                        tmp++;
                    }
                    ACCEPT(ps, '}');
                    if (ptimes) *ptimes = tmp;
                    ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
                    return PYLT_OBJ_TYPE_DICT;
                    break;
            }
            break;
    }
    return 0;
}

bool parse_basetype(ParserState *ps) {
    int ret, times;
    PyLiteObject *obj = parse_get_consttype(ps);

    if (obj) {
        sload_const(ps, obj);
        if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val = false;
        return true;
    } else {
        ret = parse_mutabletype(ps, &times);
        if (ret) {
            // NEW_OBJ TYPE SIZE
            write_ins(ps, BC_NEW_OBJ, ret, times);
        }
        return ret != 0;
    }
}

/*
T ->    ( EXPR ) |
        not EXPR |
        BASETYPE |
        EXPR[x]  |
        IDENT
*/
bool parse_try_t(ParserState *ps) {
    int num, num2, tk_val;
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;
    PyLiteInstruction ins;
    pl_bool_t old_disable_expr_tuple_parse;

    switch (tk->val) {
        case TK_NAME:
            obj = tk->obj;
            next(ps);
            switch (tk->val) {
                default:
                    if (ps->lval_check.enable && ps->lval_check.expr_level == 1)
                        write_ins(ps, BC_LOAD_VAL_EX, 0, store_const(ps, obj));
                    else
                        write_ins(ps, BC_LOAD_VAL, 0, store_const(ps, obj));
            }
            break;
        case '(':
            next(ps);
            parse_expr(ps);
            // try tuple
            if (tk->val == ',') {
                next(ps);
                num = 1;
                while (true) {
                    if (!parse_try_expr(ps)) break;
                    num++;
                    if (tk->val != ',') break;
                    next(ps);
                }
                write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_TUPLE, num);
            }
            ACCEPT(ps, ')');
            break;
        case '+': case '-':
            tk_val = tk->val;
            next(ps);
            parse_expr10(ps);
            write_ins(ps, BC_OPERATOR, 0, tk_val == '+' ? OP_POS : OP_NEG);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val = false;
            break;
        case '~':
            next(ps);
            parse_expr10(ps);
            write_ins(ps, BC_OPERATOR, 0, OP_BITNOT);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val = false;
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
            write_ins(ps, BC_OPERATOR, 0, OP_NOT);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val = false;
            break;
        default:
            if (!parse_basetype(ps))
                return false;
    }

    while (true) {
        switch (tk->val) {
            case '.':
                // is getattr/setattr ?
                next(ps);
                if (tk->val == TK_NAME) {
                    if (ps->lval_check.enable && ps->lval_check.expr_level == 1) {
                        ps->lval_check.can_be_left_val = true;
                        write_ins(ps, BC_GET_ATTR_EX, 0, store_const(ps, tk->obj));
                    } else write_ins(ps, BC_GET_ATTR, 0, store_const(ps, tk->obj));
                }
                ACCEPT(ps, TK_NAME);
                break;
            case '[':
                // is getitem/setitem ?
                next(ps);
                parse_expr(ps);
                ACCEPT(ps, ']');
                if (ps->lval_check.enable && ps->lval_check.expr_level == 1) {
                    write_ins(ps, BC_GET_ITEM_EX, 0, 0);
                    ps->lval_check.can_be_left_val = true;
                } else  write_ins(ps, BC_GET_ITEM, 0, 0);
                break;
            case '(':
                // is func call ?
                next(ps);
                num = num2 = 0;
                ins = kv_top(ps->info->code->opcodes);

                // method trigger
                if (ins.code == BC_GET_ATTR || ins.code == BC_GET_ATTR_EX) {
                    kv_top(ps->info->code->opcodes).exarg = 1;
                }

                old_disable_expr_tuple_parse = ps->disable_expr_tuple_parse;
                ps->disable_expr_tuple_parse = true;
                while (true) {
                    if (!parse_try_expr(ps)) break;
                    num++;
                    if (tk->val == ',') {
                        next(ps);
                        continue;
                    } else if (tk->val == '=') {
                        // func(a=1, b=2, ...)
                        ins = kv_top(ps->info->code->opcodes);
                        num--;
                        if (ins.code == BC_LOAD_VAL || ins.code == BC_LOAD_VAL_EX) {
                            kv_top(ps->info->code->opcodes).code = BC_LOADCONST;
                            num2 = 1;
                            next(ps);
                            parse_expr(ps);

                            while (true) {
                                if (tk->val != ',') break;
                                next(ps);
                                if (tk->val == TK_NAME) sload_const(ps, tk->obj);
                                else error(ps, PYLT_ERR_PARSER_NON_KEYWORD_ARG_AFTER_KEYWORD_ARG);
                                next(ps);
                                ACCEPT(ps, '=');
                                parse_expr(ps);
                                num2++;
                            }

                            write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_DICT, num2);
                            break;
                        } else {
                            error(ps, PYLT_ERR_PARSER_KEYWORD_CANT_BE_AN_EXPR);
                        }
                    } else break;
                }
                if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val = false;
                write_ins(ps, BC_CALL, (num2) ? 1 : 0, num);
                ACCEPT(ps, ')');
                ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
                break;
            default:
                goto _tail;
        }
    }

_tail:
    return true;
}

void parse_t(ParserState *ps) {
    if (!parse_try_t(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
}


/* EXPR -> EXPR10 ... EXPR1 */
bool parse_try_expr(ParserState *ps) {
    if (!parse_try_expr10(ps)) return false;
    parse_expr9(ps);
    parse_expr8(ps);
    parse_expr7(ps);
    parse_expr6(ps);
    parse_expr5(ps);
    parse_expr4(ps);
    parse_expr3(ps);
    parse_expr2(ps);
    parse_expr1(ps);

    // is tuple ?
    if (!ps->disable_expr_tuple_parse) {
        int num;
        Token *tk = &(ps->ls->token);

        if (tk->val == ',') {
            next(ps);
            num = 1;
            ps->disable_expr_tuple_parse = true;
            while (true) {
                if (!parse_try_expr(ps)) break;
                num++;
                if (tk->val != ',') break;
                next(ps);
            }
            write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_TUPLE, num);
            ps->disable_expr_tuple_parse = false;
        }
    }

    return true;
}

void parse_expr(ParserState *ps) {
    ps->lval_check.expr_level++;
    if (!parse_try_expr(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    ps->lval_check.expr_level--;
}

void parse_expr_without_tuple_parse(ParserState *ps) {
    bool old_disable_expr_tuple_parse = ps->disable_expr_tuple_parse;
    ps->disable_expr_tuple_parse = true;
    parse_expr(ps);
    ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
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
            write_ins(ps, BC_OPERATOR, 0, OP_OR);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, OP_AND);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
    write_ins(ps, BC_OPERATOR, 0, op_val);
    if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, OP_BITOR);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, OP_BITXOR);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, OP_BITAND);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, tk_val);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, token_to_op_val(tk_val));
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
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
            write_ins(ps, BC_OPERATOR, 0, token_to_op_val(tk_val));
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
            parse_expr9(ps);
            break;
    }
}

/* T (** T | ε) */
_INLINE bool parse_try_expr10(ParserState *ps) {
    if (!parse_try_t(ps)) return false;
    Token *tk = &(ps->ls->token);
    switch (tk->val) {
        case TK_OP_POW:
            next(ps);
            parse_t(ps);
            write_ins(ps, BC_OPERATOR, 0, OP_POW);
            if (ps->lval_check.enable && ps->lval_check.expr_level == 1) ps->lval_check.can_be_left_val =  false;
            break;
    }
    return true;
}

_INLINE void parse_expr10(ParserState *ps) {
    if (!parse_try_expr10(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
}

void parse_block(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    ACCEPT(ps, TK_INDENT);
    parse_stmts(ps);
    ACCEPT(ps, TK_DEDENT);
}

void loop_control_replace(ParserState *ps, int start_pos) {
    PyLiteInstruction ins;
    for (unsigned int i = start_pos; i < kv_size(ps->info->code->opcodes); ++i) {
        ins = kv_A(ps->info->code->opcodes, i);
        switch (ins.code) {
            case BC_FAKE_BREAK:
                if (ins.extra == ps->info->loop_depth + 1) {
                    ins.code = BC_JMP;
                    ins.extra = kv_size(ps->info->code->opcodes) - i;
                    kv_A(ps->info->code->opcodes, i) = ins;
                }
                break;
            case BC_FAKE_CONTINUE:
                if (ins.extra == ps->info->loop_depth + 1) {
                    ins.code = BC_JMP;
                    ins.extra = kv_size(ps->info->code->opcodes) - i - 1;
                    kv_A(ps->info->code->opcodes, i) = ins;
                }
                break;
        }
    }
}

void parse_func(ParserState *ps) {
    ParserInfo *info;
    PyLiteObject *name, *func_name;
    PyLiteObject *args_name = NULL, *kwargs_name = NULL;
    Token *tk = &(ps->ls->token);
    bool old_disable_return;
    int defval_count = 0;

    ACCEPT(ps, TK_KW_DEF);
    if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    func_name = tk->obj;
    next(ps);

    ACCEPT(ps, '(');

    PyLiteListObject* lst = pylt_obj_list_new(ps->state);

    while (true) {
        if (tk->val == TK_NAME) {
            name = tk->obj;
            pylt_obj_list_append(ps->state, lst, name);
            next(ps);
            if (tk->val == '=') {
                next(ps);
                sload_const(ps, name);
                parse_expr_without_tuple_parse(ps);
                defval_count++;
                if (tk->val == ',') next(ps);
                else break;
            } else if (tk->val == ',') {
                next(ps);
            } else {
                break;
            }
        } else if (tk->val == '*') {
            // *args
            next(ps);
            if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            if (args_name) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX); // TODO: error info fix
            pylt_obj_list_append(ps->state, lst, tk->obj);
            args_name = tk->obj;
            next(ps);
            if (tk->val != ',') break;
            next(ps);
        } else if (tk->val == TK_OP_POW) {
            // **kwargs
            next(ps);
            if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            pylt_obj_list_append(ps->state, lst, tk->obj);
            kwargs_name = tk->obj;
            next(ps);
            if (tk->val != ',') {
                // TODO:ERROR
            }
            break;
        } else break;
    }

    ACCEPT(ps, ')');
    ACCEPT(ps, ':');
    ACCEPT(ps, TK_NEWLINE);

    old_disable_return = ps->disable_return_parse;
    ps->disable_return_parse = false;
    func_push(ps);
    ACCEPT(ps, TK_INDENT);
    parse_stmts(ps);
    ACCEPT(ps, TK_DEDENT);
    sload_const(ps, castobj(pylt_obj_none_new(ps->state)));
    write_ins(ps, BC_RET, 0, 0);
    info = func_pop(ps);
    ps->disable_return_parse = old_disable_return;

    // defaults
    if (defval_count) {
        write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_DICT, defval_count);
    } else {
        sload_const(ps, castobj(&PyLiteNone));
    }
    // name
    sload_const(ps, func_name);
    // code
    sload_const(ps, castobj(info->code));
    // params
    sload_const(ps, castobj(lst));
    // args_name
    sload_const(ps, args_name ? args_name : castobj(&PyLiteNone));
    // kwargs_name
    sload_const(ps, kwargs_name ? kwargs_name : castobj(&PyLiteNone));

    write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_FUNCTION, 0);
    write_ins(ps, BC_SET_VAL, 0, store_const(ps, func_name));
    write_ins(ps, BC_POP, 0, 0);
}


void parse_class(ParserState *ps) {
    ParserInfo *info;
    PyLiteObject *class_name;
    PyLiteObject *base_class_name = NULL;
    Token *tk = &(ps->ls->token);
    PyLiteListObject* lst = pylt_obj_list_new(ps->state);
    bool old_disable_return;

    ACCEPT(ps, TK_KW_CLASS);
    if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    class_name = tk->obj;
    next(ps);

    if (tk->val == '(') {
        next(ps);
        if (tk->val == TK_NAME) {
            base_class_name = tk->obj;
            next(ps);
        }
        ACCEPT(ps, ')');
    }

    ACCEPT(ps, ':');
    ACCEPT(ps, TK_NEWLINE);

    old_disable_return = ps->disable_return_parse;
    ps->disable_return_parse = true;
    func_push(ps);
    ACCEPT(ps, TK_INDENT);
    parse_stmts(ps);
    ACCEPT(ps, TK_DEDENT);
    write_ins(ps, BC_LOADLOCALS, 0, 0);
    write_ins(ps, BC_RET, 0, 0);
    info = func_pop(ps);
    ps->disable_return_parse = old_disable_return;

    // defaults
    sload_const(ps, castobj(&PyLiteNone));
    // name
    sload_const(ps, castobj(class_name));
    // code
    sload_const(ps, castobj(info->code));
    // params
    sload_const(ps, castobj(lst));
    // args_name
    sload_const(ps, castobj(&PyLiteNone));
    // kwargs_name
    sload_const(ps, castobj(&PyLiteNone));

    write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_FUNCTION, 0);

    write_ins(ps, BC_CALL, 0, 0);
    if (base_class_name) write_ins(ps, BC_LOAD_VAL, 0, store_const(ps, base_class_name));
    else sload_const(ps, castobj(&PyLiteNone));
    sload_const(ps, castobj(class_name));
    write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_TYPE, 0);
    write_ins(ps, BC_SET_VAL, 0, store_const(ps, class_name));
    write_ins(ps, BC_POP, 0, 0);
}


void value_assign_fix(ParserState *ps) {
    PyLiteInstruction ins, last_ins = { 0 };

    for (pl_int_t i = kv_size(ps->lval_check.bc_cache) - 1; i >= 0;) {
        ins = kv_A(ps->lval_check.bc_cache, i);
        switch (ins.code) {
            case BC_GET_ITEM_EX:
            case BC_GET_ATTR_EX:
                kv_A(ps->lval_check.bc_cache, i).code = (ins.code == BC_GET_ITEM_EX) ? BC_SET_ITEM : BC_SET_ATTR;
                i -= 1;
                while (i >= 0) {
                    ins = kv_A(ps->lval_check.bc_cache, i);
                    if (ins.code == BC_LOAD_VAL_EX) {
                        kv_A(ps->lval_check.bc_cache, i).code = BC_LOAD_VAL;
                        i -= 1;
                        break;
                    }
                    i -= 1;
                }
                break;
            case BC_LOAD_VAL_EX:
                kv_A(ps->lval_check.bc_cache, i--).code = BC_SET_VAL;
                break;
            default:
                i--;
        }
    }
}


void parse_inplace_op(ParserState *ps, int op_val) {
    pl_uint_t size;

    next(ps);

    if (!ps->lval_check.can_be_left_val)
        error(ps, PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL);

    size = kv_size(ps->info->code->opcodes) - ps->lval_check.startcode;

    if (ps->lval_check.bc_cache.m < size) {
        kv_resize(PyLiteInstruction, ps->lval_check.bc_cache, size);
    }

    ps->lval_check.bc_cache.n = size;
    memcpy(ps->lval_check.bc_cache.a, ps->info->code->opcodes.a + ps->lval_check.startcode, sizeof(PyLiteInstruction) * size);

    parse_expr(ps);
    value_assign_fix(ps);

    write_ins(ps, BC_OPERATOR, 0, op_val);

    for (pl_uint_t i = 0; i < kv_size(ps->lval_check.bc_cache); ++i) {
        kv_pushins(ps->info->code->opcodes, kv_A(ps->lval_check.bc_cache, i));
    }

    kv_clear(ps->lval_check.bc_cache);
}

void parse_value_assign(ParserState *ps) {
    pl_uint_t size;

    //while (true) { ACCEPT(ps, '='); parse_expr(ps); break; }
    //return;

    if (ps->ls->token.val != '=')
        error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

    while (true) {
        if (ps->ls->token.val != '=') break;
        next(ps);

        if (!ps->lval_check.can_be_left_val)
            error(ps, PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL);

        size = kv_size(ps->info->code->opcodes) - ps->lval_check.startcode;

        if (ps->lval_check.bc_cache.m < size) {
            kv_resize(PyLiteInstruction, ps->lval_check.bc_cache, size);
        }

        ps->lval_check.bc_cache.n = size;
        memcpy(ps->lval_check.bc_cache.a, ps->info->code->opcodes.a + ps->lval_check.startcode, sizeof(PyLiteInstruction) * size);
        kv_popn(ps->info->code->opcodes, size);

        parse_expr(ps);
        value_assign_fix(ps);

        for (pl_uint_t i = 0; i < kv_size(ps->lval_check.bc_cache); ++i) {
            kv_pushins(ps->info->code->opcodes, kv_A(ps->lval_check.bc_cache, i));
        }

        kv_clear(ps->lval_check.bc_cache);
    }
}


void parse_stmt(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;
    int tmp, tmp2, tmp3;
    int final_pos;

    switch (tk->val) {
        case TK_KW_IF:
            next(ps);
            parse_expr(ps);
            ACCEPT(ps, ':');
            // test 0 X
            write_ins(ps, BC_TEST, 0, 0);
            tmp = tmp3 = kv_size(ps->info->code->opcodes);
            if (tk->val == TK_NEWLINE) {
                next(ps);
                parse_block(ps);
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            // TODO: stmt

            kv_A(ps->info->code->opcodes, tmp - 1).extra = kv_size(ps->info->code->opcodes) - tmp;

            tmp2 = 0;
            while (tk->val == TK_KW_ELIF) {
                next(ps);

                // jmp 0 X
                kv_A(ps->info->code->opcodes, tmp - 1).extra += 1;
                write_ins(ps, BC_JMP, 0, 0);
                tmp2 = kv_size(ps->info->code->opcodes);

                parse_expr(ps);
                ACCEPT(ps, ':');
                // test 0 X
                write_ins(ps, BC_TEST, 0, 0);
                tmp = kv_size(ps->info->code->opcodes);

                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

                // write X for jmp X
                kv_A(ps->info->code->opcodes, tmp2 - 1).extra = kv_size(ps->info->code->opcodes) - tmp2 + 1;
                // write X for test X
                kv_A(ps->info->code->opcodes, tmp - 1).extra = kv_size(ps->info->code->opcodes) - tmp;
            }

            if (tk->val == TK_KW_ELSE) {
                next(ps);
                ACCEPT(ps, ':');
                kv_A(ps->info->code->opcodes, tmp - 1).extra += 1;
                // jmp 0 X
                write_ins(ps, BC_JMP, 0, 0);
                tmp = kv_size(ps->info->code->opcodes);

                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

                // write X for jmp X
                kv_A(ps->info->code->opcodes, tmp - 1).extra = kv_size(ps->info->code->opcodes) - tmp;
            } else {
                // fix for last elif
                if (tmp2) {
                    kv_A(ps->info->code->opcodes, tmp - 1).extra = kv_size(ps->info->code->opcodes) - tmp;
                }
            }

            // new code pos
            final_pos = kv_size(ps->info->code->opcodes);
            // tmp3 <- pos of first jmp (if exists)
            tmp3 += kv_A(ps->info->code->opcodes, tmp3 - 1).extra;

            // change every jmp point
            while (tmp3 != final_pos) {
                // tmp2 <- pos of next jmp (if exists) or block end
                tmp2 = tmp3 + kv_A(ps->info->code->opcodes, tmp3 - 1).extra;
                kv_A(ps->info->code->opcodes, tmp3 - 1).extra = final_pos - tmp3;
                tmp3 = tmp2;
            }
            return;
        case TK_KW_WHILE:
            next(ps);
            tmp = kv_size(ps->info->code->opcodes);
            parse_expr(ps);
            ACCEPT(ps, ':');

            tmp2 = kv_size(ps->info->code->opcodes);
            write_ins(ps, BC_TEST, 0, 0);

            if (tk->val == TK_NEWLINE) {
                next(ps);
                ++ps->info->loop_depth;
                parse_block(ps);
                --ps->info->loop_depth;
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            loop_control_replace(ps, tmp2);

            write_ins(ps, BC_JMP_BACK, 0, (kv_size(ps->info->code->opcodes) - tmp + 1));
            kv_A(ps->info->code->opcodes, tmp2).extra = kv_size(ps->info->code->opcodes) - tmp2;
            return;
        case TK_KW_BREAK:
            next(ps);
            if (ps->info->loop_depth == 0) error(ps, PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP);
            else write_ins(ps, BC_FAKE_BREAK, 0, ps->info->loop_depth);
            break;
        case TK_KW_CONTINUE:
            next(ps);
            if (ps->info->loop_depth == 0) error(ps, PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP);
            else write_ins(ps, BC_FAKE_CONTINUE, 0, ps->info->loop_depth);
            break;
        case TK_KW_FOR:
            next(ps);
            if (tk->val == TK_NAME) {
                obj = tk->obj;
                next(ps);
                ACCEPT(ps, TK_KW_IN);
                parse_expr(ps);
                ACCEPT(ps, ':');
                write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_ITER, 0);
                tmp = kv_size(ps->info->code->opcodes);

                // for X
                write_ins(ps, BC_FORITER, 0, 0);
                write_ins(ps, BC_SET_VAL, 0, store_const(ps, obj));
                write_ins(ps, BC_POP, 0, 0);

                ++ps->info->loop_depth;
                ACCEPT(ps, TK_NEWLINE);
                parse_block(ps);
                --ps->info->loop_depth;
                loop_control_replace(ps, tmp);

                kv_A(ps->info->code->opcodes, tmp).extra = kv_size(ps->info->code->opcodes) - tmp + 1;
                write_ins(ps, BC_JMP_BACK, 0, kv_size(ps->info->code->opcodes) - tmp + 1);

                write_ins(ps, BC_DEL_FORCE, 0, 0);
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            return;
        case TK_KW_PASS:
            next(ps);
            break;
        case TK_KW_DEF:
            parse_func(ps);
            return;
        case TK_KW_CLASS:
            parse_class(ps);
            return;
        case TK_KW_IMPORT:
            next(ps);
            tmp = 0;
            while (true) {
                if (tk->val == TK_NAME) {
                    tmp++;
                    sload_const(ps, tk->obj);
                    next(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
                if (tk->val == '.') next(ps);
                else break;
            }
            write_ins(ps, BC_IMPORT_NAME, 0, tmp);
            break;
        case TK_KW_RETURN:
            if (ps->disable_return_parse) {
                error(ps, PYLT_ERR_PARSER_RETURN_OUTSIDE_FUNCTION);
            }
            next(ps);
            if (!parse_try_expr(ps)) {
                sload_const(ps, castobj(pylt_obj_none_new(ps->state)));
            }
            write_ins(ps, BC_RET, 0, 0);
            break;
        case TK_KW_ASSERT:
            next(ps);
            parse_expr(ps);
            write_ins(ps, BC_ASSERT, 0, 0);
            break;
        case TK_NEWLINE:
            // empty file
            break;
        default:
            ps->lval_check.enable = true;
            ps->lval_check.can_be_left_val = true;
            ps->lval_check.startcode = kv_size(ps->info->code->opcodes);

            parse_expr(ps);
            switch (tk->val) {
                case '=':
                    parse_value_assign(ps);
                    break;
                case TK_DE_PLUS_EQ: case TK_DE_MINUS_EQ:  case TK_DE_MUL_EQ: case TK_DE_DIV_EQ:
                case TK_DE_FLOORDIV_EQ: case TK_DE_MOD_EQ: case TK_DE_MATMUL_EQ:
                case TK_DE_BITAND_EQ: case TK_DE_BITOR_EQ: case TK_DE_BITXOR_EQ:
                case TK_DE_RSHIFT_EQ: case TK_DE_LSHIFT_EQ: case TK_DE_POW_EQ:
                    parse_inplace_op(ps, token_de_to_op_val(tk->val));
                    break;
            }

            ps->lval_check.enable = false;
            write_ins(ps, BC_POP, 0, 0);
    }
    ACCEPT(ps, TK_NEWLINE);
}


void parse_stmts(ParserState *ps) {
    while (ps->ls->token.val != TK_END && ps->ls->token.val != TK_DEDENT) {
        parse_stmt(ps);
    }
}

PyLiteCodeObject* pylt_parser_parse(ParserState *ps) {
    PyLiteCodeObject *ret;
    next(ps);
    parse_stmts(ps);
    write_ins(ps, BC_PRINT, 0, 0);
    write_ins(ps, BC_HALT, 0, 0);
    ret = ps->info->code;
    func_pop(ps);
    return ret;
}

void func_push(ParserState *ps) {
    ParserInfo *info;
    // 如果没有空槽
    if (!ps->info_used) {
        info = pylt_realloc(NULL, sizeof(ParserInfo));
    // 如果有用过的空槽
    } else {
        info = ps->info_used;
        ps->info_used = ps->info_used->prev;
    }

    // 初始化
    info->code = pylt_obj_code_new(ps->state);
    info->loop_depth = 0;
    info->prev = ps->info;
    ps->info = info;
}

ParserInfo* func_pop(ParserState *ps) {
    ParserInfo *info = ps->info;
    ps->info = ps->info->prev;

    info->prev = ps->info_used;
    ps->info_used = info;
    return info;
}

void pylt_parser_init(PyLiteState* state, ParserState *ps, LexState *ls) {
    ps->state = state;
    ps->ls = ls;

    ps->info = NULL;
    ps->info_used = NULL;

    ps->lval_check.enable = false;
    ps->lval_check.expr_level = 0;
    kv_init(ps->lval_check.bc_cache);

    ps->disable_expr_tuple_parse = false;
    ps->disable_return_parse = true;

    func_push(ps);
}

void pylt_parser_finalize(PyLiteState* state, ParserState *ps) {
    ParserInfo *info, *info2;

    info = ps->info;
    while (info) {
        info2 = info->prev;
        pylt_obj_code_free(state, info->code);
        pylt_free(info);
        info = info2;
    }

    info = ps->info_used;
    while (info) {
        info2 = info->prev;
        pylt_obj_code_free(state, info->code);
        pylt_free(info);
        info = info2;
    }

    kv_destroy(ps->lval_check.bc_cache);
}
