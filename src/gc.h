
#ifndef PYLITE_GC_H
#define PYLITE_GC_H

#include "config.h"
#include "types/set.h"

#define up_hash_func(state, key) (pl_uint32_t)(key)
#define up_hash_equal(state, a, b) ((a) == (b))

KHASHO_INIT(unique_ptr, PyLiteObject*, char, 0, up_hash_func, up_hash_equal);
typedef khasho_t(unique_ptr) PyLiteUPSet;


typedef struct PyLiteGC {
    PyLiteUPSet *white;
    PyLiteUPSet *grey;
    PyLiteUPSet *black;
    PyLiteUPSet *statics;
} PyLiteGC;

void pylt_gc_init(PyLiteState *state);
void pylt_gc_finalize(PyLiteState *state);
void pylt_gc_add(PyLiteState *state, PyLiteObject *obj);
void pylt_gc_collect(PyLiteState *state);

void pylt_gc_static_add(PyLiteState *state, PyLiteObject *obj);
void pylt_gc_static_release(PyLiteState *state);

#endif
