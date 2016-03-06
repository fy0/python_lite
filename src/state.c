
#include "state.h"

void pylt_state_err(PyLiteState *state) {
    exit(-1);
}

void pylt_state_init(PyLiteState *state) {
    kv_init(state->cls_base);
    state->modules = pylt_obj_table_new(state);
    state->cache_str = pylt_obj_set_new(state);
    state->cache_bytes = pylt_obj_set_new(state);
} 

PyLiteObject* pylt_state_obj_get(PyLiteState* state, pl_int_t n_args, pl_int_t i) {
    return castobj(kv_A(state->vm.stack, kv_size(state->vm.stack) - n_args + i));
}
