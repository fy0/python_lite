
#ifndef PYLITE_TYPES_CODE_SNIPPET_H
#define PYLITE_TYPES_CODE_SNIPPET_H

#include "object.h"
#include "../lib/kvec.h"

typedef struct PyLiteInstruction {
    uint8_t code;
    uint8_t extarg;
    int16_t extra;
} PyLiteInstruction;

typedef struct PyLiteCodeSnippetObject {
    PyLiteObject_HEAD;
    kvec_t(PyLiteObject*) const_val;
    kvec_t(PyLiteInstruction) opcodes;
} PyLiteCodeSnippetObject;

PyLiteCodeSnippetObject* pylt_obj_code_snippet_new(PyLiteState *state);

#endif
