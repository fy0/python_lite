
#ifndef PYLITE_API_H
#define PYLITE_API_H

#include "utils.h"
#include "types/all.h"

void pylt_obj_output_str(PyLiteState *state, PyLiteStrObject *obj);
PyLiteTypeObject* pylt_gettype(PyLiteState *state, pl_uint32_t type_code);

const char* pylt_obj_type_name_cstr(PyLiteState *state, PyLiteObject *obj);
const char* pylt_type_name(int ob_type);


#endif
