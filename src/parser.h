
#ifndef PYLITE_PARSER_H
#define PYLITE_PARSER_H

#include "lexer.h"
#include "deps/kvec.h"
#include "types/all.h"

typedef struct ParserInfo {
    int loop_depth;
    PyLiteCodeObject *code;
    struct ParserInfo *prev;
} ParserInfo;

typedef struct ParserState {
    LexState *ls;
    PyLiteInterpreter *I;

    ParserInfo *info;
    ParserInfo *info_used;

    struct {
        bool enable;
        bool can_be_left_val;
        pl_int_t startcode;
        pl_int_t expr_level;
        kvec_t(PyLiteInstruction) bc_cache;
    } lval_check;

    kvec_t(PyLiteDictObject*) var_tables;

    bool disable_op_in_parse;
    bool disable_expr_tuple_parse;
    bool disable_return_parse;
} ParserState;

PyLiteCodeObject* pylt_parser_parse(ParserState *ps);
ParserState* pylt_parser_new(PyLiteInterpreter *I, LexState *ls);
void pylt_parser_init(PyLiteInterpreter *I, ParserState *ps, LexState *ls);
void pylt_parser_reset(PyLiteInterpreter *I, ParserState *ps, LexState *ls);
void pylt_parser_finalize(PyLiteInterpreter *I, ParserState *ps);
void pylt_parser_crash_finalize(PyLiteInterpreter *I, ParserState *ps);

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
// default 'except:' must be last
#define PYLT_ERR_PARSER_EXCEPT_MUST_BE_LAST -9
// expected 'finally' or 'except', got invalid token
#define PYLT_ERR_PARSER_TRY_EXPECTED_FINALLY -10


// SyntaxError: different unpack sequences' size
#define PYLT_ERR_PARSER_DIFFERENT_UNPACK_SEQUENCES_SIZE -19
// SyntaxError: not enough values to unpack (expected A, got B)
#define PYLT_ERR_PARSER_NOT_ENOUGH_VALUES_TO_UNPACK -20

#endif
