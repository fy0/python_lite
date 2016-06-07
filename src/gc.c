
#include "gc.h"
#include "state.h"

void pylt_gc_add(PyLiteState *state, PyLiteObject *obj) {
    pylt_obj_set_add(state, state->gc.g_new, obj);

}

void pylt_gc_init(PyLiteState *state) {
    state->gc.g_old = pylt_obj_set_new(state);
    state->gc.g_new = pylt_obj_set_new(state);
}

void pylt_gc_finalize(PyLiteState *state) {
    pylt_obj_set_free(state, state->gc.g_old);
    pylt_obj_set_free(state, state->gc.g_new);
}
