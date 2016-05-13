
#ifndef PYLITE_DEBUG_H
#define PYLITE_DEBUG_H

#include "parser.h"

void debug_print_const_vals(PyLiteState *state, ParserState *ps);
void debug_print_opcodes(PyLiteState *state, ParserState *ps);
void debug_print_obj(PyLiteState *state, PyLiteObject *obj);

void debug_test_lexer(PyLiteState *state, StringStream *ss);

#endif
