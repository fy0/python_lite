
#ifndef PYLITE_TYPES_CODE_SNIPPET_H
#define PYLITE_TYPES_CODE_SNIPPET_H

#include "../object.h"
#include "../common/list.h"
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

	pl_bool_t with_debug_info;
	kvec_t(uint32_t) lnotab;
} PyLiteCodeObject;

void pylt_obj_code_add_to_gc(PyLiteInterpreter *I, PyLiteCodeObject *self);

PyLiteCodeObject* pylt_obj_code_new(PyLiteInterpreter *I, pl_bool_t with_debug_info);
void pylt_obj_code_free(PyLiteInterpreter *I, PyLiteCodeObject* self);

#endif
