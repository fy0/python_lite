
#include "code.h"
#include "../../gc.h"


void pylt_obj_code_add_to_gc(PyLiteInterpreter *I, PyLiteCodeObject *self) {
    PyLiteObject *obj;
    PyLiteListObject *lst = self->const_val;
    for (pl_int_t i = 0; i < lst->ob_size; ++i) {
        obj = lst->ob_val[i];
        if (pl_iscode(obj)) pylt_obj_code_add_to_gc(I, castcode(obj));
        else pylt_gc_add(I, obj);
    }
    pylt_gc_add(I, castobj(self));
}

PyLiteCodeObject* pylt_obj_code_new(PyLiteInterpreter *I, pl_bool_t with_debug_info) {
	PyLiteCodeObject *obj = pylt_malloc(I, sizeof(PyLiteCodeObject));
    obj->ob_type = PYLT_OBJ_TYPE_CODE;
    obj->const_val = pylt_obj_list_new(I);
    kv_init(I, obj->opcodes);
	obj->with_debug_info = with_debug_info;
	if (with_debug_info) {
        kv_init(I, obj->lnotab);
	}
    return obj;
}

void pylt_obj_code_free(PyLiteInterpreter *I, PyLiteCodeObject* self) {
    pylt_obj_list_free(I, self->const_val);
    kv_destroy(self->opcodes);
    pylt_free_ex(I, self);
}
