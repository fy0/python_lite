
#ifndef PYLITE_PARSER_H
#define PYLITE_PARSER_H

#include "lexer.h"
#include "lib/kvec.h"
#include "types/object.h"

typedef struct ParserState {
    LexState *ls;
    PyLiteState* state;
    kvec_t(PyLiteObject*) const_val;
    kvec_t(size_t) opcodes;
} ParserState;

#define kv_pushobj(v, x) kv_push(PyLiteObject*, (v), (x))
#define kv_pushbc(v, x) kv_push(size_t, (v), (x))

void parse(ParserState *ps);
void pylt_parser_init(ParserState *ps, PyLiteState* state, LexState *ls);

#endif
