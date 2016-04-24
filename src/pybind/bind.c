
#include "bind.h"
#include "common.h"
#include "../state.h"
#include "../types/all.h"
#include "../mods/helper.h"

void pylt_obj_type_register(PyLiteState *state, PyLiteTypeObject* type) {
    kv_A(state->cls_base, type->ob_reftype) = type;
    pylt_obj_type_setattr(state, type, castobj(_NS(state, "__base__")), castobj(kv_A(state->cls_base, type->ob_base)));
}

void pylt_bind_all_types_register(PyLiteState *state) {
    PyLiteTypeObject *type;
    kv_resize(PyLiteTypeObject*, state->cls_base, PYLT_OBJ_TYPE_NUM + 10);

    // object
    type = pylt_obj_type_new(state, PTLT_OBJ_TYPE_OBJ, 0);
    pylt_cmethod_register_0_args(state, type, _NS(state, "mro"), &pylt_method_obj_mro);
    pylt_obj_type_register(state, type);

    // int
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_INT, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // float
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_FLOAT, PTLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_0_args(state, type, _NS(state, "is_integer"), &pylt_method_int_is_integer);
    pylt_obj_type_register(state, type);

    // bool
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_BOOL, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 5 ==============

    // str
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_STR, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // bytes
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_BYTES, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // set
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_SET, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // list
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_LIST, PTLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_1_args(state, type, _NS(state, "append"), &pylt_method_list_append);
    pylt_cmethod_register_0_args(state, type, _NS(state, "clear"), &pylt_method_list_clear);
    pylt_cmethod_register_0_args(state, type, _NS(state, "copy"), &pylt_method_list_copy);
    pylt_cmethod_register_0_args(state, type, _NS(state, "count"), &pylt_method_list_count);
    pylt_cmethod_register(state, type, _NS(state, "extend"), _NST(state, 2, "self", "object"), NULL, _UINTS(2, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_LIST), &pylt_method_list_extend);
    pylt_cmethod_register_1_args(state, type, _NS(state, "index"), &pylt_method_list_index);
    pylt_cmethod_register(state, type, _NS(state, "insert"), _NST(state, 3, "self", "index", "object"), NULL, _UINTS(3, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_INT, 0), &pylt_method_list_insert);
    pylt_cmethod_register(state, type, _NS(state, "pop"), _NST(state, 2, "self", "index"), _NT(state, 2, 0, pylt_obj_int_new(state, -1)), _UINTS(2, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_INT), &pylt_method_list_pop);
    pylt_cmethod_register_1_args(state, type, _NS(state, "remove"), &pylt_method_list_remove);
    pylt_cmethod_register_0_args(state, type, _NS(state, "reverse"), &pylt_method_list_reverse);
    pylt_obj_type_register(state, type);

    // tuple
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_TUPLE, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // dict
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_DICT, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 11 ==============

    // module
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_MODULE, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // function
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_FUNCTION, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // cfunction
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_CFUNCTION, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // code
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_CODESNIPPET, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 15 ==============

    // type
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_TYPE, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // iter
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_ITER, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // class
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_CLASS, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // none
    type = pylt_obj_type_new(state, PYLT_OBJ_TYPE_NONE, PTLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);
}
