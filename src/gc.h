
#ifndef PYLITE_GC_H
#define PYLITE_GC_H

#include "misc.h"
#include "types/set.h"

typedef struct PyLiteGC {
    PyLiteSetObject *g_old;
    PyLiteSetObject *g_new;
} PyLiteGC;

void pylt_gc_init(PyLiteState *state);
void pylt_gc_finalize(PyLiteState *state);
void pylt_gc_add(PyLiteState *state, PyLiteObject *obj);

#endif
