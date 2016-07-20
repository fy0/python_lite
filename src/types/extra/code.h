
#ifndef PYLITE_TYPES_CODE_SNIPPET_H
#define PYLITE_TYPES_CODE_SNIPPET_H

#include "../object.h"
#include "../list.h"
#include "../../lib/kvec.h"

typedef struct PyLiteInstruction {
    uint8_t code;
    uint8_t exarg;
    int16_t extra;
} PyLiteInstruction;

typedef struct PyLiteCodeObject {
    PyLiteObject_HEAD;
    PyLiteListObject *const_val;
    kvec_t(PyLiteInstruction) opcodes;
} PyLiteCodeObject;

void pylt_obj_code_add_to_gc(PyLiteState* state, PyLiteCodeObject *self);

PyLiteCodeObject* pylt_obj_code_new(PyLiteState *state);
void pylt_obj_code_free(PyLiteState *state, PyLiteCodeObject* self);

#endif
