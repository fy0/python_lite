
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "vm.h"
#include "gc.h"
#include "lexer.h"
#include "parser.h"
#include "deps/kvec.h"
#include "types/common/set.h"
#include "types/common/type.h"
#include "utils/io/_io.h"

typedef struct {
    PyLiteIO *defio;
    PyLiteFS *deffs;
#ifdef PYLT_EXTMOD
    PyLiteFile *cin;
    PyLiteFile *cout;
    PyLiteFile *cerr;
#endif
} PyLiteSys;

typedef struct PyLiteInterpreter {
    PyLiteVM vm;
    PyLiteGC gc;
    LexState *lexer;
    ParserState *parser;
    PyLiteSys sys;

    // loaded modules
    PyLiteDictObject *modules;
    // keys are modpaths(os/re/test.a) and values are their load functions
    PyLiteDictObject *cmodules_loader;
    // exception object when error occurred. NULL as default.
    PyLiteObject *error;
    // the last called func object
    PyLiteObject *recent_called;

    pl_uint_t mem_used;
    pl_int_t class_num;
    kvec_t(PyLiteTypeObject*) cls_base;
} PyLiteInterpreter;

PyLiteInterpreter* pylt_intp_new();
void pylt_intp_free(PyLiteInterpreter *I);

void pylt_intp_init(PyLiteInterpreter *I);
void pylt_intp_finalize(PyLiteInterpreter *I);
void pylt_intp_loadf(PyLiteInterpreter *I, PyLiteFile *input);
PyLiteCodeObject* pylt_intp_parsef(PyLiteInterpreter *I, PyLiteFile *input);

#endif
