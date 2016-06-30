
#ifndef PYLITE_GC_H
#define PYLITE_GC_H

#include "misc.h"
#include "types/set.h"

typedef struct PyLiteGC {
    PyLiteSetObject *white;
    PyLiteSetObject *grey;
    PyLiteSetObject *black;
    PyLiteSetObject *statics;
} PyLiteGC;

void pylt_gc_init(PyLiteState *state);
void pylt_gc_finalize(PyLiteState *state);
void pylt_gc_add(PyLiteState *state, PyLiteObject *obj);
void pylt_gc_local_add(PyLiteState *state, PyLiteObject *obj);
void pylt_gc_static_add(PyLiteState *state, PyLiteObject *obj);

void pylt_gc_local_release(PyLiteState *state);
void pylt_gc_static_release(PyLiteState *state);

pl_bool_t pylt_gc_isstatic(PyLiteState *state, PyLiteObject *obj);

#endif
