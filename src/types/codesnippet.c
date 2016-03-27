
#include "codesnippet.h"

PyLiteCodeSnippetObject* pylt_obj_code_snippet_new(PyLiteState *state) {
    PyLiteCodeSnippetObject *obj = pylt_realloc(NULL, sizeof(PyLiteCodeSnippetObject));
    obj->ob_type = PYLT_OBJ_TYPE_CODESNIPPET;
    kv_init(obj->const_val);
    kv_init(obj->opcodes);
    return obj;
}
