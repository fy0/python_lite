
#include "code.h"

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
