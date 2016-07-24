
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "gc.h"
#include "lib/kvec.h"
#include "types/set.h"
#include "types/type.h"

typedef struct PyLiteInterpreter {
    PyLiteVM vm;
    PyLiteGC gc;
    LexState *lexer;
    ParserState *parser;

    kvec_t(PyLiteTypeObject*) cls_base;
    PyLiteDictObject *modules;
    pl_int_t class_num;
    int error_code;
} PyLiteInterpreter;

PyLiteInterpreter* pylt_intp_new();
void pylt_intp_free(PyLiteInterpreter *I);

void pylt_intp_init(PyLiteInterpreter *I);
void pylt_intp_finalize(PyLiteInterpreter *I);
void pylt_intp_run(PyLiteInterpreter *I);
void pylt_intp_load_stream(PyLiteInterpreter *I, StringStream *ss);
//void pylt_intp_err(PyLiteInterpreter *I);

#endif
