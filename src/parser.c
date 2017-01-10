
#include "intp.h"
#include "parser.h"
#include "debug.h"
#include "vm.h"
#include "api.h"
#include "bind.h"
#include "types/all.h"
#include "utils/misc.h"

#define kv_pushobj(v, x) kv_push(PyLiteObject*, (v), (PyLiteObject*)(x))
#define kv_pushins(v, x) kv_push(PyLiteInstruction, (v), (x))

void func_push(ParserState *ps);
ParserInfo* func_pop(ParserState *ps);
void func_scope_check(ParserState *ps, PyLiteListObject *args);

void parse_t(ParserState *ps);

void parse_stmts(ParserState *ps);
void parse_block(ParserState *ps);

void parse_expr(ParserState *ps);
void parse_expr1(ParserState *ps);  // or
void parse_expr2(ParserState *ps);  // and
void parse_expr3(ParserState *ps);  // (<|<=|>|>=|!=|==|in|not in|is|is not)
void parse_expr4(ParserState *ps);  // |
void parse_expr5(ParserState *ps);  // ^
void parse_expr6(ParserState *ps);  // &
void parse_expr7(ParserState *ps);  // (<<|>>)
void parse_expr8(ParserState *ps);  // (+|-)
void parse_expr9(ParserState *ps);  // (*|@|/|//|%)
void parse_expr10(ParserState *ps); // T (** T | ε)

bool parse_try_t(ParserState *ps);
bool parse_try_expr(ParserState *ps);
bool parse_try_expr10(ParserState *ps);

void parse_func(ParserState *ps);
void parse_lambda(ParserState *ps);
void parse_class(ParserState *ps);
void parse_value_assign(ParserState *ps);
void parse_inplace_op(ParserState *ps, int op_val);

void lval_check_setup(ParserState *ps);
void lval_check_shutdown(ParserState *ps);
bool lval_check_judge(ParserState *ps, bool val);
pl_uint_t lval_check_cache_push(ParserState *ps);
void lval_check_cache_pop(ParserState *ps);

void error(ParserState *ps, int code);

void next(ParserState *ps) {
    int ret = pylt_lex_next(ps->ls);
    if (ret < 0) {
        error(ps, ret);
    }
    ps->token_count++;
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
    kv_push(uint32_t, ps->info->code->lnotab, (uint32_t)(ps->ls->linenumber));
    kv_pushins(ps->info->code->opcodes, ins);
}

int store_const(ParserState *ps, PyLiteObject *obj) {
    PyLiteListObject *const_val = ps->info->code->const_val;
    pl_int_t index = pylt_obj_list_index_strict(ps->I, const_val, obj);
    if (index == -1) index = pylt_obj_list_append(ps->I, const_val, obj);
    return index;
}

void sload_const(ParserState *ps, PyLiteObject *obj) {
    if (obj == castobj(&PyLiteNone)) write_ins(ps, BC_LOADNONE, 0, 0);
    else write_ins(ps, BC_LOADCONST, 0, store_const(ps, obj));
}

void sload_name(ParserState *ps, PyLiteStrObject *obj) {
    write_ins(ps, BC_LOAD_VAL, 0, store_const(ps, castobj(obj)));
}

void sload_none(ParserState *ps) {
    write_ins(ps, BC_LOADNONE, 0, 0);
}

void error(ParserState *ps, int code) {
    Token *tk = &(ps->ls->token);
    wprintf(L"ERROR at line [%d]\n", ps->ls->linenumber);
    //if (tk->val == TK_INT) raw_str_print(&(tk->str));
    const wchar_t *name = pylt_lex_get_token_name(tk->val);
    if (name) wprintf(L"%ls", name);
    else putwchar(tk->val);
    putwchar('\n');
    switch (code) {
        case PYLT_ERR_PARSER_INVALID_SYNTAX:
            wprintf(L"SyntaxError: invalid syntax\n");
            break;
        case PYLT_ERR_PARSER_BYTES_INVALID_ESCAPE:
            wprintf(L"SyntaxError: (value error) invalid escape\n");
            break;
        case PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP:
            wprintf(L"SyntaxError: 'break' outside loop\n");
            break;
        case PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP:
            wprintf(L"SyntaxError: 'continue' not properly in loop\n");
            break;
        case PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL:
            wprintf(L"SyntaxError: can't assign to literal\n");
            break;
        case PYLT_ERR_PARSER_RETURN_OUTSIDE_FUNCTION:
            wprintf(L"SyntaxError: 'return' outside function\n");
            break;
        case PYLT_ERR_PARSER_EXCEPT_MUST_BE_LAST:
            wprintf(L"SyntaxError: default 'except:' must be last\n");
            break;
        case PYLT_ERR_PARSER_TRY_EXPECTED_FINALLY:
            wprintf(L"SyntaxError: expected 'finally' or 'except', got invalid token\n");
            break;
        case PYLT_ERR_PARSER_KEYWORD_CANT_BE_AN_EXPR:
            wprintf(L"SyntaxError: keyword can't be an expression\n");
            break;
        case PYLT_ERR_LEX_INVALID_STR_OR_BYTES:
            wprintf(L"SyntaxError: bad string literal\n");
            break;
    }
    system("pause");
    exit(-1);
}

#define OPCODE_TOP(ps) kv_top(ps->info->code->opcodes)
#define OPCODE_SIZE(ps) kv_size((ps)->info->code->opcodes)
#define OPCODE_GET(ps, pos) kv_A((ps)->info->code->opcodes, pos)
#define OPCODE_PTR(ps, pos) kv_P((ps)->info->code->opcodes, pos)

static _INLINE
void ACCEPT(ParserState *ps, int token) {
    if (ps->ls->token.val != token) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    next(ps);
}

void lval_check_setup(ParserState *ps) {
    ps->lval_check.enable = true;
    ps->lval_check.can_be_left_val = true;
    ps->lval_check.startcode = OPCODE_SIZE(ps);
}

void lval_check_shutdown(ParserState *ps) {
    ps->lval_check.enable = false;
}

#define lval_check_valid(ps) (ps->lval_check.enable && ps->lval_check.expr_level == 1)

bool lval_check_judge(ParserState *ps, bool val) {
    if (lval_check_valid(ps)) {
        ps->lval_check.can_be_left_val = val;
        return true;
    }
    return false;
}

pl_uint_t lval_check_cache_push(ParserState *ps) {
    pl_uint_t size;

    size = OPCODE_SIZE(ps) - ps->lval_check.startcode;

    if (ps->lval_check.bc_cache.m < size) {
        kv_resize(PyLiteInstruction, ps->lval_check.bc_cache, size);
    }

    ps->lval_check.bc_cache.n = size;
    memcpy(ps->lval_check.bc_cache.a, ps->info->code->opcodes.a + ps->lval_check.startcode, sizeof(PyLiteInstruction) * size);
    return size;
}

void lval_check_cache_pop(ParserState *ps) {
    for (pl_uint_t i = 0; i < kv_size(ps->lval_check.bc_cache); ++i) {
        kv_pushins(ps->info->code->opcodes, kv_A(ps->lval_check.bc_cache, i));
    }
    kv_clear(ps->lval_check.bc_cache);
}

PyLiteObject* parse_get_consttype(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;

    switch (tk->val) {
        case TK_KW_TRUE:
            next(ps);
            return castobj(pylt_obj_bool_new(ps->I, true));
        case TK_KW_FALSE:
            next(ps);
            return castobj(pylt_obj_bool_new(ps->I, false));
        case TK_KW_NONE:
            next(ps);
            return castobj(&PyLiteNone);
        case TK_INT: case TK_FLOAT: case TK_STRING: case TK_BYTES:
            obj = tk->obj;
            next(ps);
            return obj;
        default:
            return NULL;
    }
}

// 为什么我当初没有把 tuple 也放进来？忘记了。
// 好像是因为 ( 可能是 tuple 也可能是语句的缘故吧
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
        lval_check_judge(ps, false);
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

// attr / item / slice / func call
bool parse_try_suffix(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    pl_bool_t accepted = false;
    pl_bool_t old_disable_expr_tuple_parse;

    while (true) {
        switch (tk->val) {
            case '.':
                // is getattr/setattr ?
                next(ps);
                if (tk->val == TK_NAME) {
                    write_ins(ps, lval_check_judge(ps, true) ? BC_GET_ATTR_ : BC_GET_ATTR, 0, store_const(ps, tk->obj));
                    accepted = true;
                }
                ACCEPT(ps, TK_NAME);
                break;
            case '[':
                // is getitem/setitem ?
                next(ps);

                // get slice [: ?
                if (tk->val == ':') {
                    next(ps);
                    // 补全 start
                    sload_none(ps);
                    goto slice_parse_end;
                }

                // item or start of slice
                parse_expr(ps);

                // [X:
                // slice begin
                if (tk->val == ':') {
                    next(ps);

                slice_parse_end:
                    // [X:] ?
                    if (tk->val == ']') {
                        next(ps);
                        // 补全 end 和 step
                        sload_none(ps);
                        sload_none(ps);
                        goto slice_parse_final;
                    }
                    // [X:: ?
                    if (tk->val == ':') {
                        next(ps);
                        // 补全 end
                        sload_none(ps);
                        goto slice_parse_step;
                    }

                    // [X:Y
                    parse_expr(ps);

                    // [X:Y] ?
                    if (tk->val == ']') {
                        next(ps);
                        // 补全 step
                        sload_none(ps);
                        goto slice_parse_final;
                    }
                    // [X:Y: ?
                    if (tk->val == ':') {
                        next(ps);
                        goto slice_parse_step;
                    }

                slice_parse_step:
                    // [X:Y:]
                    if (tk->val == ']') {
                        next(ps);
                        // 补全 step
                        sload_none(ps);
                        goto slice_parse_final;
                    }

                    // [X:Y:Z
                    parse_expr(ps);
                    ACCEPT(ps, ']');

                slice_parse_final:
                    write_ins(ps, lval_check_judge(ps, true) ? BC_GET_SLICE_ : BC_GET_SLICE, 0, 0);
                    accepted = true;
                    break;
                }

                // getitem/setitem
                ACCEPT(ps, ']');
                write_ins(ps, lval_check_judge(ps, true) ? BC_GET_ITEM_ : BC_GET_ITEM, 0, 0);
                accepted = true;
                break;
            case '(': {
                // is func call ?
                next(ps);
                int num = 0, num_kwargs = 0;
                PyLiteInstruction ins = kv_top(ps->info->code->opcodes);

                // method trigger
                if (ins.code == BC_GET_ATTR || ins.code == BC_GET_ATTR_) {
                    OPCODE_TOP(ps).exarg = 1;
                }

                old_disable_expr_tuple_parse = ps->disable_expr_tuple_parse;
                ps->disable_expr_tuple_parse = true;

                /* parameters order:
                    func(a, *b, d, e = 1, **f) # valid
                    func(e = 1, a) # invalid
                    func(e = 1, *a) # invalid
                    func(**f, a) # invalid
                    func(**f, e = 1) # invalid
                */

                if (tk->val == ')') goto _call_unpack_dict;

                while (true) {
                    // *
                    if (tk->val == '*') {
                        next(ps);
                        // *arg
                        if (!parse_try_expr(ps)) {
                            error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
                        }
                        // unpack and increase params_offset
                        write_ins(ps, BC_UNPACK_ARG, 0, 0);
                    } else if (tk->val == TK_OP_POW) {
                        break;
                    } else {
                        if (tk->val == TK_NAME) {
                            // a = ?
                            pl_int_t count = ps->token_count;
                            parse_expr(ps);
                            if (ps->token_count - count == 1) {
                                if (tk->val == '=') {
                                    next(ps);
                                    // func(a=1, b=2, ...)
                                    // BC_LOAD_VAL/BC_LOAD_VAL_
                                    OPCODE_TOP(ps).code = BC_LOADCONST;
                                    parse_expr(ps);
                                    num_kwargs = 1;

                                    while (true) {
                                        if (tk->val != ',') break;
                                        next(ps);
                                        if (tk->val == TK_NAME) sload_const(ps, tk->obj);
                                        else break;
                                        next(ps);
                                        ACCEPT(ps, '='); // TODO: f(a=1, b) 此处错误提示信息应更加友好
                                        parse_expr(ps);
                                        num_kwargs++;
                                    }

                                    write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_DICT, num_kwargs);
                                    goto _call_unpack_dict;
                                }
                            } else {
                                if (tk->val == '=') {
                                    error(ps, PYLT_ERR_PARSER_KEYWORD_CANT_BE_AN_EXPR);
                                }
                            }
                            num++;
                        } else {
                            // 常规表达式
                            parse_expr(ps);
                            num++;
                        }
                    }
                    if (tk->val == ',') next(ps);
                    if (tk->val == ')') break;
                }

            _call_unpack_dict:
                if (tk->val == TK_OP_POW) {
                    next(ps);
                    // **kwargs
                    // must be the last
                    parse_expr(ps);
                    if (num_kwargs) write_ins(ps, BC_DICT_COMBINE, 0, 0);
                    else num_kwargs = 1;
                    if (tk->val == ',') next(ps);
                }
                ACCEPT(ps, ')');

                lval_check_judge(ps, false);
                write_ins(ps, BC_CALL, (num_kwargs) ? 1 : 0, num);
                ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
                break;
            }
            default:
                return accepted;
        }
    }
    return accepted;
}

/*
T ->    ( EXPR ) |
        not EXPR |
        BASETYPE |
        EXPR[x]  |
        lambda   |
        IDENT
*/
bool parse_try_t(ParserState *ps) {
    int num, tk_val;
    Token *tk = &(ps->ls->token);
    PyLiteObject *obj;

    switch (tk->val) {
        case TK_NAME:
            obj = tk->obj;
            next(ps);
            write_ins(ps, lval_check_valid(ps) ? BC_LOAD_VAL_ : BC_LOAD_VAL, 0, store_const(ps, obj));
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
            lval_check_judge(ps, false);
            break;
        case '~':
            next(ps);
            parse_expr10(ps);
            write_ins(ps, BC_OPERATOR, 0, OP_BITNOT);
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
            break;
        case TK_KW_LAMBDA:
            parse_lambda(ps);
            lval_check_judge(ps, false);
            break;
        default:
            if (!parse_basetype(ps))
                return false;
    }

    parse_try_suffix(ps);
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
    Token *tk = &(ps->ls->token);
    bool add_expr_level = (tk->val != '[' && tk->val != '(');
    // 这里回避的情况是：[b,a] = 1,2
    // 这时候需要生成 BC_LOAD_VAL_ 才能顺利完成后面的批量赋值
    if (add_expr_level) ps->lval_check.expr_level++;
    if (!parse_try_expr(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
    if (add_expr_level) ps->lval_check.expr_level--;
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
            parse_expr2(ps);
            break;
    }
}


/* (<|<=|>|>=|!=|==|in|not in|is|is not) EXPR10 ... EXPR3 | ε */
_INLINE void parse_expr3(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tk_val, op_val;
    switch (tk->val) {
        case TK_KW_IN:
            if (ps->disable_op_in_parse && ps->lval_check.expr_level <= 1) {
                return;
            }
        case '<': case TK_OP_LE: case '>': case TK_OP_GE: case TK_OP_NE: case TK_OP_EQ:
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
    lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
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
            lval_check_judge(ps, false);
            break;
    }
    return true;
}

_INLINE void parse_expr10(ParserState *ps) {
    if (!parse_try_expr10(ps)) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
}

void parse_block(ParserState *ps) {
    ACCEPT(ps, TK_INDENT);
    parse_stmts(ps);
    ACCEPT(ps, TK_DEDENT);
}

void loop_control_replace(ParserState *ps, int start_pos) {
    PyLiteInstruction ins;
    for (unsigned int i = start_pos; i < OPCODE_SIZE(ps); ++i) {
        ins = kv_A(ps->info->code->opcodes, i);
        switch (ins.code) {
            case BC_PH_BREAK:
                if (ins.extra == ps->info->loop_depth + 1) {
                    ins.code = BC_JMP;
                    ins.extra = OPCODE_SIZE(ps) - i;
                    kv_A(ps->info->code->opcodes, i) = ins;
                }
                break;
            case BC_PH_CONTINUE:
                if (ins.extra == ps->info->loop_depth + 1) {
                    ins.code = BC_JMP;
                    ins.extra = OPCODE_SIZE(ps) - i - 1;
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

    PyLiteListObject* lst = pylt_obj_list_new(ps->I);

    while (true) {
        if (tk->val == TK_NAME) {
            name = tk->obj;
            pylt_obj_list_append(ps->I, lst, name);
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
            pylt_obj_list_append(ps->I, lst, tk->obj);
            args_name = tk->obj;
            next(ps);
            if (tk->val != ',') break;
            next(ps);
        } else if (tk->val == TK_OP_POW) {
            // **kwargs
            next(ps);
            if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            pylt_obj_list_append(ps->I, lst, tk->obj);
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
    write_ins(ps, BC_RET, 0, 0);
    func_scope_check(ps, lst);
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
    PyLiteListObject* lst = pylt_obj_list_new(ps->I);
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
    write_ins(ps, BC_RET, 0, 1);
    func_scope_check(ps, lst);
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


int value_assign_fix(ParserState *ps) {
    int si, seqsize = 0;
    PyLiteInstruction ins, last_ins = { 0 };

    for (pl_int_t i = kv_size(ps->lval_check.bc_cache) - 1; i >= 0;) {
        ins = kv_A(ps->lval_check.bc_cache, i);
        switch (ins.code) {
            case BC_GET_ITEM_:
            case BC_GET_ATTR_:
            case BC_GET_SLICE_:
                kv_A(ps->lval_check.bc_cache, i).code += 1; // + 1 = BC_SET_ITEM / BC_SET_ATTR / BC_SET_SLICE
                kv_A(ps->lval_check.bc_cache, i).exarg = seqsize ? si++ : 0;
                i--;
                while (i >= 0) {
                    ins = kv_A(ps->lval_check.bc_cache, i);
                    if (ins.code == BC_LOAD_VAL_) {
                        kv_A(ps->lval_check.bc_cache, i).code = BC_LOAD_VAL;
                        i -= 1;
                        break;
                    }
                    i -= 1;
                }
                break;
            case BC_LOAD_VAL_:
                if (seqsize) {
                    kv_A(ps->lval_check.bc_cache, i).code = BC_SET_VALX;
                    kv_A(ps->lval_check.bc_cache, i--).exarg = si++;
                } else {
                    kv_A(ps->lval_check.bc_cache, i--).code = BC_SET_VAL;
                }
                break;
            case BC_NEW_OBJ:
                // lval is tuple, unpack sequence
                si = 0;
                seqsize = kv_A(ps->lval_check.bc_cache, i).extra;
                kv_A(ps->lval_check.bc_cache, i--).code = BC_NOP;
                //kv_pop(ps->lval_check.bc_cache);
                break;
            default:
                i--;
        }
    }
    return seqsize;
}


void parse_inplace_op(ParserState *ps, int op_val) {
    pl_uint_t size;
    next(ps);

    if (!ps->lval_check.can_be_left_val)
        error(ps, PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL);

    size = lval_check_cache_push(ps);
    parse_expr(ps);
    value_assign_fix(ps);

    write_ins(ps, BC_OPERATOR, 0, op_val);
    lval_check_cache_pop(ps);
}

void parse_value_assign(ParserState *ps) {
    pl_uint_t size;
    int last_seqsize = 0;

    if (ps->ls->token.val != '=')
        error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

    while (true) {
        if (ps->ls->token.val != '=') break;
        next(ps);

        if (!ps->lval_check.can_be_left_val)
            error(ps, PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL);

        size = lval_check_cache_push(ps);
        kv_popn(ps->info->code->opcodes, size);

        parse_expr(ps);
        int seqsize = value_assign_fix(ps);
        if (last_seqsize && (seqsize != last_seqsize)) {
            error(ps, PYLT_ERR_PARSER_DIFFERENT_UNPACK_SEQUENCES_SIZE);
        }
        last_seqsize = seqsize;

        // is right val ?
        if (ps->ls->token.val != '=') {
            if (last_seqsize) {
                PyLiteInstruction ins = kv_top(ps->info->code->opcodes);
                // tuple ? list ?
                if (ins.code == BC_NEW_OBJ && (ins.exarg == PYLT_OBJ_TYPE_TUPLE || ins.exarg == PYLT_OBJ_TYPE_LIST)) {
                    kv_pop(ps->info->code->opcodes);
                    if (ins.extra != last_seqsize) {
                        error(ps, PYLT_ERR_PARSER_NOT_ENOUGH_VALUES_TO_UNPACK);
                    }
                } else {
                    write_ins(ps, BC_UNPACK_SEQ, 0, last_seqsize);
                }
            }
        }

        lval_check_cache_pop(ps);
    }

    if (last_seqsize) write_ins(ps, BC_POPN, 0, last_seqsize);
    else write_ins(ps, BC_POP, 0, 0);
}

pl_int_t parse_try_del_item(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    pl_int_t codesize = OPCODE_SIZE(ps);
    int num, state = 0;
    PyLiteObject *obj;

    switch (tk->val) {
        case TK_NAME:
            obj = tk->obj;
            next(ps);
            write_ins(ps, lval_check_valid(ps) ? BC_LOAD_VAL_ : BC_LOAD_VAL, 0, store_const(ps, obj));
            state = 1; // LOAD VAL
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
                state = 2; // container
            }
            ACCEPT(ps, ')');
            break;
        default: {
            int ret, times;
            ret = parse_mutabletype(ps, &times);
            if (ret) {
                // NEW_OBJ TYPE SIZE
                write_ins(ps, BC_NEW_OBJ, ret, times);
                state = 2; // container
            }
        }
    }
    if (state == 0) {
        kv_popn(ps->info->code->opcodes, OPCODE_SIZE(ps) - codesize);
        // SyntaxError: can't delete literal
        return PYLT_ERR_PARSER_INVALID_SYNTAX;
    }

    parse_try_suffix(ps);

    PyLiteInstruction ins = kv_top(ps->info->code->opcodes);
    switch (ins.code) {
        case BC_LOAD_VAL:
        case BC_LOAD_VAL_:
            kv_top(ps->info->code->opcodes).code = BC_DEL_NAME;
            break;
        case BC_GET_ATTR:
        case BC_GET_ATTR_:
            kv_top(ps->info->code->opcodes).code = BC_DEL_ATTR;
            break;
        case BC_GET_ITEM:
        case BC_GET_ITEM_:
            kv_top(ps->info->code->opcodes).code = BC_DEL_ITEM;
            break;
        case BC_GET_SLICE:
        case BC_GET_SLICE_:
            kv_top(ps->info->code->opcodes).code = BC_DEL_SLICE;
            break;
        default:
            // SyntaxError: can't delete literal
            kv_popn(ps->info->code->opcodes, OPCODE_SIZE(ps) - codesize);
            return PYLT_ERR_PARSER_INVALID_SYNTAX;
    }
    return 0;
}

void parse_del(ParserState *ps) {
    pl_bool_t bracket = false;
    Token *tk = &(ps->ls->token);
    next(ps);

    pl_int_t ret = parse_try_del_item(ps);
    if (ret) error(ps, ret);

    while (true) {
        if (tk->val == ',') next(ps);
        else break;
        if (tk->val == TK_NEWLINE) break;
        else {
            ret = parse_try_del_item(ps);
            if (ret) error(ps, ret);
        }
    }
}

void parse_lambda(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    ACCEPT(ps, TK_KW_LAMBDA);

    PyLiteObject *name;
    PyLiteObject *args_name = NULL, *kwargs_name = NULL;
    PyLiteListObject *params = pylt_obj_list_new(ps->I);
    int defval_count = 0;

    while (true) {
        if (tk->val == TK_NAME) {
            name = tk->obj;
            pylt_obj_list_append(ps->I, params, name);
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
            pylt_obj_list_append(ps->I, params, tk->obj);
            args_name = tk->obj;
            next(ps);
            if (tk->val != ',') break;
            next(ps);
        } else if (tk->val == TK_OP_POW) {
            // **kwargs
            next(ps);
            if (tk->val != TK_NAME) error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            pylt_obj_list_append(ps->I, params, tk->obj);
            kwargs_name = tk->obj;
            next(ps);
            if (tk->val != ',') {
                // TODO:ERROR
            }
            break;
        } else break;
    }
    if (tk->val == ',') next(ps);
    ACCEPT(ps, ':');

    pl_bool_t old_disable_return = ps->disable_return_parse;
    ps->disable_return_parse = false;
    func_push(ps);
    parse_expr(ps);
    write_ins(ps, BC_RET, 0, 1);
    func_scope_check(ps, params);
    ParserInfo *info = func_pop(ps);
    ps->disable_return_parse = old_disable_return;

    // defaults
    if (defval_count) write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_DICT, defval_count);
    else write_ins(ps, BC_LOADNONE, 0, 0);

    sload_const(ps, castobj(pl_static.str.TMPL_LAMBDA_FUNC_NAME)); // name
    sload_const(ps, castobj(info->code)); // code
    sload_const(ps, castobj(params)); // params
    sload_const(ps, args_name ? args_name : castobj(&PyLiteNone));  // args_name    
    sload_const(ps, kwargs_name ? kwargs_name : castobj(&PyLiteNone)); // kwargs_name

    write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_FUNCTION, 0);
}

void parse_stmt(ParserState *ps) {
    Token *tk = &(ps->ls->token);
    int tmp, tmp2, tmp3;
    int final_pos;
    //pl_bool_t old;

    switch (tk->val) {
        case TK_KW_IF:
            next(ps);
            // if 1,2,3: pass 语法错
            // 这是一个好特性吗？
            //old = ps->disable_expr_tuple_parse;
            //ps->disable_expr_tuple_parse = true;
            parse_expr(ps);
            //ps->disable_expr_tuple_parse = old;
            ACCEPT(ps, ':');
            // test 0 X
            write_ins(ps, BC_TEST, 0, 0);
            tmp = tmp3 = OPCODE_SIZE(ps);
            if (tk->val == TK_NEWLINE) {
                next(ps);
                parse_block(ps);
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            // TODO: stmt

            OPCODE_GET(ps, tmp - 1).extra = OPCODE_SIZE(ps) - tmp;

            tmp2 = 0;
            while (tk->val == TK_KW_ELIF) {
                next(ps);

                // jmp 0 X
                OPCODE_GET(ps, tmp - 1).extra += 1;
                write_ins(ps, BC_JMP, 0, 0);
                tmp2 = OPCODE_SIZE(ps);

                parse_expr(ps);
                ACCEPT(ps, ':');
                // test 0 X
                write_ins(ps, BC_TEST, 0, 0);
                tmp = OPCODE_SIZE(ps);

                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

                // write X for jmp X
                kv_A(ps->info->code->opcodes, tmp2 - 1).extra = OPCODE_SIZE(ps) - tmp2 + 1;
                // write X for test X
                OPCODE_GET(ps, tmp - 1).extra = OPCODE_SIZE(ps) - tmp;
            }

            if (tk->val == TK_KW_ELSE) {
                next(ps);
                ACCEPT(ps, ':');
                OPCODE_GET(ps, tmp - 1).extra += 1;
                // jmp 0 X
                write_ins(ps, BC_JMP, 0, 0);
                tmp = OPCODE_SIZE(ps);

                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

                // write X for jmp X
                OPCODE_GET(ps, tmp - 1).extra = OPCODE_SIZE(ps) - tmp;
            } else {
                // fix for last elif
                if (tmp2) {
                    OPCODE_GET(ps, tmp - 1).extra = OPCODE_SIZE(ps) - tmp;
                }
            }

            // new code pos
            final_pos = OPCODE_SIZE(ps);
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
            tmp = OPCODE_SIZE(ps);
            parse_expr(ps);
            ACCEPT(ps, ':');

            tmp2 = OPCODE_SIZE(ps);
            write_ins(ps, BC_TEST, 0, 0);

            if (tk->val == TK_NEWLINE) {
                next(ps);
                ++ps->info->loop_depth;
                parse_block(ps);
                --ps->info->loop_depth;
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            loop_control_replace(ps, tmp2);

            write_ins(ps, BC_JMP_BACK, 0, (OPCODE_SIZE(ps) - tmp + 1));
            OPCODE_GET(ps, tmp2).extra = OPCODE_SIZE(ps) - tmp2 - 1;
            return;
        case TK_KW_BREAK:
            next(ps);
            if (ps->info->loop_depth == 0) error(ps, PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP);
            else write_ins(ps, BC_PH_BREAK, 0, ps->info->loop_depth);
            break;
        case TK_KW_CONTINUE:
            next(ps);
            if (ps->info->loop_depth == 0) error(ps, PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP);
            else write_ins(ps, BC_PH_CONTINUE, 0, ps->info->loop_depth);
            break;
        case TK_KW_FOR: {
            pl_uint_t size;
            next(ps);

            // left value
            lval_check_setup(ps);
            ps->disable_op_in_parse = true;
            parse_expr(ps);
            ps->disable_op_in_parse = false;
            size = lval_check_cache_push(ps);
            kv_popn(ps->info->code->opcodes, size);

            // in [right_value]
            ACCEPT(ps, TK_KW_IN);
            parse_expr(ps);
            ACCEPT(ps, ':');
            write_ins(ps, BC_NEW_OBJ, PYLT_OBJ_TYPE_ITER, 0);
            tmp = OPCODE_SIZE(ps);

            // for X in iter
            write_ins(ps, BC_FORITER, 0, 0);
            int seqsize = value_assign_fix(ps);
            if (seqsize) {
                write_ins(ps, BC_UNPACK_SEQ, 0, seqsize);
            }

            lval_check_cache_pop(ps);
            write_ins(ps, (seqsize) ? BC_POPN : BC_POP, 0, seqsize);

            ++ps->info->loop_depth;
            ACCEPT(ps, TK_NEWLINE);
            parse_block(ps);
            --ps->info->loop_depth;
            loop_control_replace(ps, tmp);

            OPCODE_GET(ps, tmp).extra = OPCODE_SIZE(ps) - tmp;
            write_ins(ps, BC_JMP_BACK, 0, OPCODE_SIZE(ps) - tmp + 1);

            write_ins(ps, BC_POP, 0, 0); // pop iterator
            write_ins(ps, BC_DEL_FORCE, 0, 0);
            return;
        }
        case TK_KW_PASS:
            next(ps);
            break;
        case TK_KW_DEF:
            parse_func(ps);
            return;
        case TK_KW_CLASS:
            parse_class(ps);
            return;
        case TK_KW_DEL:
            parse_del(ps);
            break;
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
            if (parse_try_expr(ps)) {
                write_ins(ps, BC_RET, 0, 1);
            } else {
                write_ins(ps, BC_RET, 0, 0);
            }
            break;
        case TK_KW_ASSERT: {
            bool old_disable_expr_tuple_parse = ps->disable_expr_tuple_parse;
            ps->disable_expr_tuple_parse = true;
            next(ps);
            parse_expr(ps);
            if (tk->val == ',') {
                next(ps);
                parse_expr(ps);
                write_ins(ps, BC_ASSERT, 0, 1);
            } else {
                write_ins(ps, BC_ASSERT, 0, 0);
            }
            ps->disable_expr_tuple_parse = old_disable_expr_tuple_parse;
            break;
        }
        case TK_KW_TRY: {
            struct TryBox {
                pl_uint_t jmp_pos;
                PyLiteStrObject *expt_name;
            };
            kvec_t(struct TryBox) jmp_stack;
            kv_init(ps->I, jmp_stack);

            // try:
            next(ps);
            ACCEPT(ps, ':');
            write_ins(ps, BC_LOAD_VAL, 0, 0); // default: BaseException
            int first_setup = OPCODE_SIZE(ps);
            write_ins(ps, BC_EXPT_SETUP, 0, 0);

            if (tk->val == TK_NEWLINE) {
                next(ps);
                parse_block(ps);
            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);

            pl_bool_t base_expt_catched = false;

            while (true) {
                PyLiteStrObject *expt_name = NULL;
                PyLiteStrObject *alias = NULL;

                // except
                if (tk->val == TK_KW_EXCEPT) {
                    next(ps);
                    expt_name = pl_static.str.BaseException;
                    // except Exception
                    if (tk->val == TK_NAME) {
                        expt_name = caststr(tk->obj);
                        next(ps);
                        // except Exception as
                        if (tk->val == TK_KW_AS) {
                            next(ps);
                            // except Exception as x
                            if (tk->val == TK_NAME) {
                                alias = caststr(tk->obj);
                                next(ps);
                            } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
                        }
                    }

                    // except:
                    // except Exception:
                    // except Exception as x:
                    ACCEPT(ps, ':');
                    ACCEPT(ps, TK_NEWLINE);

                    if (expt_name == pl_static.str.BaseException) {
                        if (base_expt_catched) {
                            error(ps, PYLT_ERR_PARSER_EXCEPT_MUST_BE_LAST);
                        }
                        base_expt_catched = true;
                    }

                    // jmp to finally
                    struct TryBox tb = {
                        .jmp_pos = OPCODE_SIZE(ps),
                        .expt_name = expt_name,
                    };
                    kv_push(struct TryBox, jmp_stack, tb);
                    write_ins(ps, BC_JMP, 0, 0);

                    // load x
                    pl_int_t aindex;
                    if (alias) {
                        aindex = store_const(ps, castobj(alias));
                        write_ins(ps, BC_SET_VAL, 0, aindex);
                    }
                    write_ins(ps, BC_POP, 0, 0);
                    parse_block(ps);
                    if (alias) write_ins(ps, BC_DEL_NAME, 0, aindex);
                } else break;
            }

            pl_uint_t stack_size = kv_size(jmp_stack);
            switch (stack_size) {
                case 0:
                    if (tk->val != TK_KW_FINALLY) error(ps, PYLT_ERR_PARSER_TRY_EXPECTED_FINALLY);
                    OPCODE_GET(ps, first_setup - 1).code = BC_NOP;
                    OPCODE_GET(ps, first_setup).code = BC_NOP;
                    break;
                case 1: {
                    struct TryBox tb = kv_pop(jmp_stack);
                    OPCODE_GET(ps, first_setup - 1).extra = store_const(ps, castobj(tb.expt_name));
                    OPCODE_GET(ps, first_setup).extra = tb.jmp_pos - first_setup;
                    OPCODE_GET(ps, tb.jmp_pos).extra = tb.jmp_pos - first_setup;
                    break;
                }
                default: {
                    pl_uint_t n = stack_size;
                    pl_uint_t offset = (n - 1) * 2;
                    
                    pl_uint_t move_size = OPCODE_SIZE(ps) - (first_setup + 1);
                    for (pl_uint_t i = 0; i < offset; ++i) write_ins(ps, BC_NOP, 0, 0); // 临时方案

                    PyLiteInstruction *pins = OPCODE_PTR(ps, first_setup + 1);
                    memmove(pins + offset, pins, sizeof(PyLiteInstruction) * move_size);

                    for (pl_uint_t i = 0; i < n; ++i) {
                        struct TryBox tb = kv_pop(jmp_stack);
                        // exception setup
                        OPCODE_GET(ps, first_setup + i * 2 - 1).code = BC_LOAD_VAL;
                        OPCODE_GET(ps, first_setup + i * 2 - 1).extra = store_const(ps, castobj(tb.expt_name));
                        OPCODE_GET(ps, first_setup + i * 2).code = BC_EXPT_SETUP;
                        OPCODE_GET(ps, first_setup + i * 2).extra = tb.jmp_pos - first_setup + offset - i * 2;
                        // finally
                        OPCODE_GET(ps, tb.jmp_pos + offset).extra = OPCODE_SIZE(ps) - tb.jmp_pos - offset - 1;
                    }
                    break;
                }
            }

            write_ins(ps, BC_EXPT_POPN, 0, stack_size);
            if (tk->val == TK_KW_FINALLY) {
                next(ps);
                ACCEPT(ps, ':');
                if (tk->val == TK_NEWLINE) {
                    next(ps);
                    parse_block(ps);
                } else error(ps, PYLT_ERR_PARSER_INVALID_SYNTAX);
            }
            return;
        }
        case TK_KW_RAISE: {
            next(ps);
            if (parse_try_expr(ps)) write_ins(ps, BC_RAISE, 0, 1);
            else write_ins(ps, BC_RAISE, 0, 0);
            break;
        }
        case TK_NEWLINE:
            // empty file
            break;
        default:
            lval_check_setup(ps);
            parse_expr(ps);
            switch (tk->val) {
                case '=':
                    // 这里需要自行处理 POP 的参数个数
                    parse_value_assign(ps);
                    lval_check_shutdown(ps);
                    goto _end;
                case TK_DE_PLUS_EQ: case TK_DE_MINUS_EQ:  case TK_DE_MUL_EQ: case TK_DE_DIV_EQ:
                case TK_DE_FLOORDIV_EQ: case TK_DE_MOD_EQ: case TK_DE_MATMUL_EQ:
                case TK_DE_BITAND_EQ: case TK_DE_BITOR_EQ: case TK_DE_BITXOR_EQ:
                case TK_DE_RSHIFT_EQ: case TK_DE_LSHIFT_EQ: case TK_DE_POW_EQ:
                    parse_inplace_op(ps, token_de_to_op_val(tk->val));
                    break;
            }

            lval_check_shutdown(ps);
            write_ins(ps, BC_POP, 0, 0);
    }
_end:
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
#ifdef PL_DEBUG_INFO
    write_ins(ps, BC_PRINT, 0, 0);
#endif
    write_ins(ps, BC_HALT, 0, 0);
    ret = ps->info->code;
    func_scope_check(ps, NULL);
    func_pop(ps);
    return ret;
}

void func_push(ParserState *ps) {
    ParserInfo *info;
    // 如果没有空槽
    if (!ps->info_used) {
        info = pylt_malloc(ps->I, sizeof(ParserInfo));
    // 如果有用过的空槽
    } else {
        info = ps->info_used;
        ps->info_used = ps->info_used->prev;
    }

    // 初始化
    info->code = pylt_obj_code_new(ps->I, true);
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

void func_scope_check(ParserState *ps, PyLiteListObject *args) {
    PyLiteInstruction ins;
    PyLiteCodeObject *code = ps->info->code;
    PyLiteSetObject *storeset = pylt_obj_set_new(ps->I);
    PyLiteSetObject *upvalue = pylt_obj_set_new(ps->I);

    if (args) {
        for (pl_int_t i = 0; i < args->ob_size; ++i) {
            pylt_obj_set_add(ps->I, storeset, args->ob_val[i]);
        }
    }

    for (unsigned int i = 0; i < kv_size(code->opcodes); ++i) {
        ins = kv_A(code->opcodes, i);
        switch (ins.code) {
            case BC_LOAD_VAL:
            case BC_LOAD_VAL_: {
                PyLiteObject *name = code->const_val->ob_val[ins.extra];
                if (!pylt_obj_set_has(ps->I, storeset, name)) {
                    pylt_obj_set_add(ps->I, upvalue, castobj(name));
                }
                break;
            }
            case BC_SET_VAL:
            case BC_SET_VALX: {
                PyLiteObject *name = code->const_val->ob_val[ins.extra];
                pylt_obj_set_add(ps->I, storeset, name);
                break;
            }
        }
    }

    code->closure = upvalue;
    pylt_obj_set_free(ps->I, storeset);
}

ParserState* pylt_parser_new(PyLiteInterpreter *I, LexState *ls) {
    ParserState *parser = pylt_malloc(I, sizeof(ParserState));
    pylt_parser_init(I, parser, ls);
    return parser;
}

void pylt_parser_init(PyLiteInterpreter *I, ParserState *ps, LexState *ls) {
    ps->I = I;
    ps->ls = ls;

    ps->info = NULL;
    ps->info_used = NULL;

    ps->lval_check.enable = false;
    ps->lval_check.expr_level = 0;
    kv_init(I, ps->lval_check.bc_cache);

    ps->disable_op_in_parse = false;
    ps->disable_expr_tuple_parse = false;
    ps->disable_return_parse = true;
    ps->token_count = 0;

    func_push(ps);
}

void pylt_parser_reset(PyLiteInterpreter *I, ParserState *ps, LexState *ls) {
    pylt_parser_finalize(I, ps);
    pylt_parser_init(I, ps, ls);
}

// release memory if compile failed
void pylt_parser_crash_finalize(PyLiteInterpreter *I, ParserState *ps) {
    ParserInfo *info, *info2;

    info = ps->info;
    while (info) {
        info2 = info->prev;

        // free const vals
        PyLiteListObject *lst = info->code->const_val;
        for (pl_int_t i = 0; i < lst->ob_size; ++i) {
            pylt_obj_free(I, lst->ob_val[i]);
        }

        pylt_obj_code_free(I, info->code);
        pylt_free_ex(I, info);
        info = info2;
    }

    info = ps->info_used;
    while (info) {
        info2 = info->prev;
        pylt_free_ex(I, info);
        info = info2;
    }

    kv_destroy(ps->lval_check.bc_cache);
}

void pylt_parser_finalize(PyLiteInterpreter *I, ParserState *ps) {
    ParserInfo *info, *info2;

    info = ps->info_used;
    while (info) {
        info2 = info->prev;
        pylt_free_ex(I, info);
        info = info2;
    }

    kv_destroy(ps->lval_check.bc_cache);
}
