
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "parser.h"
#include "vm.h"

typedef struct PyLiteState {
    ParserState ps;
    PyLiteVM vm;
} PyLiteState;

//void pylt_state_err(PyLiteState *state);

#endif
