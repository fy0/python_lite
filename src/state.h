
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

typedef struct PyLiteState {
    int foo;
} PyLiteState;

void pylt_state_err(PyLiteState *state);

#endif
