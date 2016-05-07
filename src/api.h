
#ifndef PYLITE_API_H
#define PYLITE_API_H

#include "misc.h"
#include "types/all.h"

void pylt_api_output_str(PyLiteState *state, PyLiteStrObject *obj);

PyLiteStrObject* pylt_api_type_name(PyLiteState *state, int ob_type);
const char* pylt_api_type_name_cstr(PyLiteState *state, int ob_type);

PyLiteTypeObject* pylt_api_gettype(PyLiteState *state, pl_uint32_t type_code);
pl_bool_t pylt_api_isinstance(PyLiteState *state, PyLiteObject *obj, pl_uint32_t type_code);

#endif
