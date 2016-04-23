
#include "bind.h"
#include "../state.h"
#include "../types/all.h"

void type_register(PyLiteState *state) {
    PyLiteTypeObject *type;
    kv_resize(PyLiteTypeObject*, state->cls_base, PYLT_OBJ_TYPE_NUM + 10);

    // object
    type = pylt_obj_type_new(state, 0);
    kv_A(state->cls_base, PTLT_OBJ_TYPE_OBJ) = type;

    // int
    type = pylt_obj_type_new(state, PTLT_OBJ_TYPE_OBJ);

}
