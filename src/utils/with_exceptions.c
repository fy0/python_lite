
#include "with_exceptions.h"
#include "../api.h"

typedef PyLiteObject* (*GetItemFunc)(PyLiteInterpreter *I, PyLiteObject *self, int index);

PyLiteObject* pylt_obj_X_Egetitem(PyLiteInterpreter *I, GetItemFunc *func, PyLiteObject *self, PyLiteObject *index) {
    if (index->ob_type == PYLT_OBJ_TYPE_INT) {
        PyLiteObject *ret = (*func)(I, self, castint(index)->ob_val);
        if (!ret) pl_error(I, pl_static.str.IndexError, "%s index out of range", pl_type(I, castobj(self))->name);
        return ret;
    }
    pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(self))->name, pl_type(I, index)->name);
    return NULL;
}


PyLiteObject* pylt_obj_tuple_Egetitem(PyLiteInterpreter *I, PyLiteTupleObject *self, PyLiteObject *index) {
    return pylt_obj_X_Egetitem(I, (GetItemFunc*)&pylt_obj_tuple_getitem, castobj(self), index);
}

PyLiteObject* pylt_obj_list_Egetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index) {
    return pylt_obj_X_Egetitem(I, (GetItemFunc*)&pylt_obj_list_getitem, castobj(self), index);
}

PyLiteObject* pylt_obj_dict_Egetitem(PyLiteInterpreter *I, PyLiteDictObject *self, PyLiteObject *key) {
    PyLiteObject *ret = pylt_obj_dict_getitem(I, self, key);
    if (!ret) pl_error(I, pl_static.str.KeyError, "%s", key);
    return ret;
}


pl_bool_t pylt_obj_list_Esetitem(PyLiteInterpreter *I, PyLiteListObject *self, PyLiteObject *index, PyLiteObject* obj) {
    if (index->ob_type == PYLT_OBJ_TYPE_INT) {
        pl_bool_t ret = pylt_obj_list_setitem(I, self, castint(index)->ob_val, obj);
        if (!ret) pl_error(I, pl_static.str.IndexError, "list assignment index out of range");
        return ret;
    }
    pl_error(I, pl_static.str.TypeError, "%s indices must be integers, not %s", pl_type(I, castobj(self))->name, pl_type(I, index)->name);
    return false;
}
