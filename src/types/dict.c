
#include "dict.h"

PyLiteDictObject* pylt_obj_dict_new(PyLiteState *state) {
    PyLiteDictObject *obj = pylt_realloc(NULL, sizeof(PyLiteDictObject));
    obj->ob_type = PYLT_OBJ_TYPE_DICT;
    obj->ob_val = pylt_obj_table_new(state);
    return obj;
}
