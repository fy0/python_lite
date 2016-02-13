
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "parser.h"
#include "vm.h"
#include "lib/kvec.h"

typedef struct PyLiteState {
    ParserState ps;
    PyLiteVM vm;
    kvec_t(uint32_t) cls_base;
} PyLiteState;

void pylt_state_init(PyLiteState *state);
//void pylt_state_err(PyLiteState *state);

#endif
