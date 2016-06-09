
#include "gc.h"
#include "state.h"

void pylt_gc_add(PyLiteState *state, PyLiteObject *obj) {
    pylt_obj_set_add(state, state->gc.g_new, obj);
}

void pylt_gc_init(PyLiteState *state) {
    state->gc.g_old = pylt_obj_set_new(state);
    state->gc.g_new = pylt_obj_set_new(state);
    state->gc.g_local = pylt_obj_set_new(state);
    state->gc.g_static = pylt_obj_set_new(state);
}

void pylt_gc_finalize(PyLiteState *state) {
    pylt_gc_static_release(state);
    pylt_obj_set_free(state, state->gc.g_old);
    pylt_obj_set_free(state, state->gc.g_new);
    pylt_obj_set_free(state, state->gc.g_local);
    pylt_obj_set_free(state, state->gc.g_static);
}

void pylt_gc_static_add(PyLiteState *state, PyLiteObject *obj) {
    pylt_obj_set_add(state, state->gc.g_static, obj);
}

void pylt_gc_local_add(PyLiteState *state, PyLiteObject *obj) {
    pylt_obj_set_add(state, state->gc.g_local, obj);
}

pl_bool_t pylt_gc_isstatic(PyLiteState *state, PyLiteObject *obj) {
    return pylt_obj_set_has(state, state->gc.g_static, obj);
}

void pylt_gc_local_release(PyLiteState *state) {
    PyLiteSetObject *set = state->gc.g_local;
    for (pl_int_t k = pylt_obj_set_begin(state, set); k != pylt_obj_set_end(state, set); pylt_obj_set_next(state, set, &k)) {
        pylt_obj_free(state, pylt_obj_set_itemvalue(state, set, k));
    }
}

void pylt_gc_static_release(PyLiteState *state) {
    PyLiteSetObject *set = state->gc.g_static;
    for (pl_int_t k = pylt_obj_set_begin(state, set); k != pylt_obj_set_end(state, set); pylt_obj_set_next(state, set, &k)) {
        pylt_obj_free(state, pylt_obj_set_itemvalue(state, set, k));
    }
}
