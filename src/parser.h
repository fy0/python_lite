
#ifndef PYLITE_PARSER_H
#define PYLITE_PARSER_H

#include "lexer.h"
#include "lib/kvec.h"
#include "types/all.h"

typedef struct ParserInfo {
    int loop_depth;
    PyLiteCodeObject *code;
    struct ParserInfo *prev;
} ParserInfo;

typedef struct ParserState {
    LexState *ls;
    PyLiteState* state;

    ParserInfo *info;
    ParserInfo *info_used;

    struct {
        bool enable;
        bool can_be_left_val;
        pl_int_t startcode;
        pl_int_t expr_level;
        kvec_t(PyLiteInstruction) bc_cache;
    } lval_check;

    bool disable_expr_tuple_parse;
    bool disable_return_parse;
} ParserState;
 
#define kv_pushobj(v, x) kv_push(PyLiteObject*, (v), (PyLiteObject*)(x))
#define kv_pushins(v, x) kv_push(PyLiteInstruction, (v), (x))

void parse(ParserState *ps);
void pylt_parser_init(PyLiteState* state, ParserState *ps, LexState *ls);

// SyntaxError: invalid syntax
#define PYLT_ERR_PARSER_INVALID_SYNTAX -1
// SyntaxError: (value error) invalid escape
#define PYLT_ERR_PARSER_BYTES_INVALID_ESCAPE -2
// SyntaxError: 'break' outside loop
#define PYLT_ERR_PARSER_BREAK_OUTSIDE_LOOP -3
// SyntaxError: 'continue' not properly in loop
#define PYLT_ERR_PARSER_CONTINUE_OUTSIDE_LOOP -4
// SyntaxError: can't assign to literal
#define PYLT_ERR_PARSER_CANT_ASSIGN_TO_LITERAL -5
// SyntaxError: keyword can't be an expression
#define PYLT_ERR_PARSER_KEYWORD_CANT_BE_AN_EXPR -6
// SyntaxError: non-keyword arg after keyword arg
#define PYLT_ERR_PARSER_NON_KEYWORD_ARG_AFTER_KEYWORD_ARG -7
// SyntaxError: 'return' outside function
#define PYLT_ERR_PARSER_RETURN_OUTSIDE_FUNCTION -8

#endif
