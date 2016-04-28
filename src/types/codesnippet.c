
#include "codesnippet.h"

PyLiteCodeObject* pylt_obj_code_new(PyLiteState *state) {
    PyLiteCodeObject *obj = pylt_realloc(NULL, sizeof(PyLiteCodeObject));
    obj->ob_type = PYLT_OBJ_TYPE_CODE;
    kv_init(obj->const_val);
    kv_init(obj->opcodes);
    return obj;
}
