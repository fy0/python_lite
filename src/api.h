
#ifndef PYLITE_API_H
#define PYLITE_API_H

#include "utils.h"
#include "types/all.h"

void pylt_obj_output_str(PyLiteState *state, PyLiteStrObject *obj);
PyLiteTypeObject* pylt_gettype(PyLiteState *state, pl_uint32_t type_code);

#endif
