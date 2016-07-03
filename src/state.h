
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "gc.h"
#include "lib/kvec.h"
#include "types/set.h"
#include "types/type.h"

typedef struct PyLiteState {
    PyLiteVM vm;
    PyLiteGC gc;
    LexState *lexer;
    ParserState *parser;

    kvec_t(PyLiteTypeObject*) cls_base;
    PyLiteDictObject *modules;
    pl_int_t class_num;
    int error_code;
} PyLiteState;

PyLiteState* pylt_state_new();
void pylt_state_free(PyLiteState *state);

void pylt_state_init(PyLiteState *state);
void pylt_state_finalize(PyLiteState *state);
void pylt_state_run(PyLiteState *state);
void pylt_state_load_stream(PyLiteState *state, StringStream *ss);
//void pylt_state_err(PyLiteState *state);

#endif
