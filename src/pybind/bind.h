
#ifndef PYLITE_BIND_BIND_H
#define PYLITE_BIND_BIND_H

#include "../types/all.h"

void pylt_obj_type_register(PyLiteState *state, PyLiteTypeObject* type);
void pylt_bind_all_types_register(PyLiteState *state);

#endif