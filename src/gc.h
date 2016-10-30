
#ifndef PYLITE_GC_H
#define PYLITE_GC_H

#include "config.h"
#include "utils/ref.h"
#include "types/common/set.h"
#include "types/common/bytes.h"
#include "types/common/string.h"

// use "(pl_int_t)" to avoid => warning: cast from pointer to integer of different size [-Wpointer-to-int-cast]
#define up_hash_func(I, key) ((pl_uint32_t)(pl_int_t)(key))
#define up_hash_equal(I, a, b) ((a) == (b))

KHASHO_INIT(unique_ptr, PyLiteObject*, char, 0, up_hash_func, up_hash_equal);
typedef khasho_t(unique_ptr) PyLiteUPSet;


typedef struct PyLiteGC {
    // Tri-color Mark-and-sweep
    PyLiteUPSet *white;
    PyLiteUPSet *grey;
    PyLiteUPSet *black;

    // Keep reference alive
    PyLiteUPSet *refs;
    // Static containers
    PyLiteUPSet *statics;
    // Cache for str/bytes
    PyLiteSetObject *str_cached;
} PyLiteGC;

void pylt_gc_init(PyLiteInterpreter *I);
void pylt_gc_finalize(PyLiteInterpreter *I);
void pylt_gc_freeall(PyLiteInterpreter *I);
void pylt_gc_add(PyLiteInterpreter *I, PyLiteObject *obj);
void pylt_gc_remove(PyLiteInterpreter *I, PyLiteObject *obj);
void pylt_gc_collect(PyLiteInterpreter *I);

PyLiteStrObject* pylt_gc_cache_str_add(PyLiteInterpreter *I, PyLiteStrObject *key);
PyLiteBytesObject* pylt_gc_cache_bytes_add(PyLiteInterpreter *I, PyLiteBytesObject *key);

void pylt_gc_static_add(PyLiteInterpreter *I, PyLiteObject *obj);
void pylt_gc_static_remove(PyLiteInterpreter *I, PyLiteObject *obj);

void pylt_gc_ref_add(PyLiteInterpreter *I, PyLiteRef *ref);
void pylt_gc_ref_remove(PyLiteInterpreter *I, PyLiteRef *ref);

void pylt_gc_static_release(PyLiteInterpreter *I);

#endif
