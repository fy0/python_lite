
#ifndef PYLITE_DEBUG_H
#define PYLITE_DEBUG_H

#include "../parser.h"

#ifdef _MSC_VER
#ifndef _DEBUG
#undef PL_DEBUG_INFO
#endif
#endif // _MSC_VER

//#define PL_DEBUG_INFO

void debug_print_const_vals(PyLiteInterpreter *I, PyLiteCodeObject *code);
void debug_print_opcodes(PyLiteInterpreter *I, PyLiteCodeObject *code);
void debug_print_obj(PyLiteInterpreter *I, PyLiteObject *obj);

void debug_test_lexer(PyLiteInterpreter *I, PyLiteFile *input);

#endif
