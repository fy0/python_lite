
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "parser.h"
#include "vm.h"
#include "lib/kvec.h"
#include "types/set.h"
#include "types/type.h"

typedef struct PyLiteState {
    PyLiteVM vm;
    kvec_t(PyLiteTypeObject*) cls_base;
    PyLiteSetObject *cache_str;
    PyLiteSetObject *cache_bytes;
    PyLiteTable *modules;
    pl_int_t class_num;
    int error_code;
} PyLiteState;

void pylt_state_init(PyLiteState *state);
//void pylt_state_err(PyLiteState *state);

#endif
