
#include "code.h"
#include "../../gc.h"


void pylt_obj_code_add_to_gc(PyLiteState* state, PyLiteCodeObject *self) {
    PyLiteObject *obj;
    PyLiteListObject *lst = self->const_val;
    for (pl_int_t i = 0; i < lst->ob_size; ++i) {
        obj = lst->ob_val[i];
        if (pl_iscode(obj)) pylt_obj_code_add_to_gc(state, castcode(obj));
        else pylt_gc_add(state, obj);
    }
    pylt_gc_add(state, castobj(self));
}

PyLiteCodeObject* pylt_obj_code_new(PyLiteState *state) {
    PyLiteCodeObject *obj = pylt_realloc(NULL, sizeof(PyLiteCodeObject));
    obj->ob_type = PYLT_OBJ_TYPE_CODE;
    obj->const_val = pylt_obj_list_new(state);
    kv_init(obj->opcodes);
    return obj;
}

void pylt_obj_code_free(PyLiteState *state, PyLiteCodeObject* self) {
    pylt_obj_list_free(state, self->const_val);
    kv_destroy(self->opcodes);
    pylt_free(self);
}
