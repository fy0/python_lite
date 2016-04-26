﻿
#include "common.h"
#include "../api.h"
#include "../types/all.h"

PyLiteObject* pylt_method_type_mro(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteTypeObject* type;
    PyLiteListObject *lst = pylt_obj_list_new(state);
    pl_uint32_t ob_type = casttype(args[0])->ob_reftype;

    while (ob_type) {
        type = pylt_api_gettype(state, ob_type);
        pylt_obj_list_append(state, lst, castobj(type));
        ob_type = type->ob_base;
    }

    return castobj(lst);
}

PyLiteObject* pylt_prop_type_base(PyLiteState *state, int argc, PyLiteObject **args) {
    PyLiteTypeObject *type = pylt_api_gettype(state, args[0]->ob_type);
    return castobj(pylt_api_gettype(state, type->ob_base));
}


PyLiteObject* pylt_method_int_is_integer(PyLiteState *state, int argc, PyLiteObject **args) {
    double i;
    PyLiteFloatObject *self = castfloat(args[0]);
    return castobj((modf(self->ob_val, &i) == 0) ? &PyLiteTrue : &PyLiteFalse);
}



PyLiteObject* pylt_method_set_add(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_set_add(state, castset(args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_set_clear(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_set_clear(state, castset(args[0]));
    return NULL;
}

PyLiteObject* pylt_method_set_copy(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_set_copy(state, castset(args[0])));
}

PyLiteObject* pylt_method_set_pop(PyLiteState *state, int argc, PyLiteObject **args) {
    return pylt_obj_set_pop(state, castset(args[0]));
}

PyLiteObject* pylt_method_set_remove(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_set_remove(state, castset(args[0]), args[1]);
    return NULL;
}



PyLiteObject* pylt_method_list_append(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_append(state, castlist(args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_list_clear(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_clear(state, castlist(args[0]));
    return NULL;
}

PyLiteObject* pylt_method_list_copy(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_list_copy(state, castlist(args[0])));
}

PyLiteObject* pylt_method_list_count(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, pylt_obj_list_ccount(state, castlist(args[0]))));
}

PyLiteObject* pylt_method_list_extend(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_extend(state, castlist(args[0]), castlist(args[1]));
    return NULL;
}

PyLiteObject* pylt_method_list_index(PyLiteState *state, int argc, PyLiteObject **args) {
    return castobj(pylt_obj_int_new(state, pylt_obj_list_cindex(state, castlist(args[0]), args[1])));
}

PyLiteObject* pylt_method_list_insert(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_insert(state, castlist(args[0]), castint(args[1])->ob_val, args[2]);
    return NULL;
}

PyLiteObject* pylt_method_list_pop(PyLiteState *state, int argc, PyLiteObject **args) {
    return pylt_obj_list_pop(state, castlist(args[0]));
}

PyLiteObject* pylt_method_list_remove(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_remove(state, castlist(args[0]), args[1]);
    return NULL;
}

PyLiteObject* pylt_method_list_reverse(PyLiteState *state, int argc, PyLiteObject **args) {
    pylt_obj_list_reverse(state, castlist(args[0]));
    return NULL;
}

PyLiteObject* pylt_method_list_sort(PyLiteState *state, int argc, PyLiteObject **args) {
    // TODO
    return NULL;
}
