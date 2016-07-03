
#ifndef PYLITE_GC_H
#define PYLITE_GC_H

#include "config.h"
#include "types/set.h"
#include "types/bytes.h"
#include "types/string.h"

#define up_hash_func(state, key) (pl_uint32_t)(key)
#define up_hash_equal(state, a, b) ((a) == (b))

KHASHO_INIT(unique_ptr, PyLiteObject*, char, 0, up_hash_func, up_hash_equal);
typedef khasho_t(unique_ptr) PyLiteUPSet;


typedef struct PyLiteGC {
    // Tri-color Mark-and-sweep
    PyLiteUPSet *white;
    PyLiteUPSet *grey;
    PyLiteUPSet *black;

    // Cache for str/bytes
    PyLiteSetObject *str_static;
    PyLiteSetObject *str_cached;
} PyLiteGC;

void pylt_gc_init(PyLiteState *state);
void pylt_gc_finalize(PyLiteState *state);
void pylt_gc_add(PyLiteState *state, PyLiteObject *obj);
void pylt_gc_remove(PyLiteState *state, PyLiteObject *obj);
void pylt_gc_collect(PyLiteState *state);

PyLiteStrObject* pylt_gc_cache_str_add(PyLiteState *state, PyLiteStrObject *key);
PyLiteBytesObject* pylt_gc_cache_bytes_add(PyLiteState *state, PyLiteBytesObject *key);
void pylt_gc_make_str_static(PyLiteState *state, PyLiteObject *obj);

void pylt_gc_static_release(PyLiteState *state);

#endif
