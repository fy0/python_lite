
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "parser.h"
#include "vm.h"
#include "lib/kvec.h"
#include "types/set.h"

typedef struct PyLiteState {
    PyLiteVM vm;
    kvec_t(uint32_t) cls_base;
    PyLiteSetObject *cache_str;
    PyLiteSetObject *cache_bytes;
    PyLiteTable *modules;
    int error_code;
} PyLiteState;

void pylt_state_init(PyLiteState *state);
//void pylt_state_err(PyLiteState *state);

PyLiteObject* pylt_state_obj_get(PyLiteState* state, pl_int_t n_args, pl_int_t i);

#endif
