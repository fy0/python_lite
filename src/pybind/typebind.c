
#include "../bind.h"
#include "../state.h"
#include "../types/all.h"
#include "common.h"
#include "extra.h"


void pylt_bind_all_types_register(PyLiteState *state) {
    PyLiteTypeObject *type;
    kv_resize(PyLiteTypeObject*, state->cls_base, PYLT_OBJ_BUILTIN_TYPE_NUM + 10);

    // object
    type = pylt_obj_type_new(state, pl_static.str.object, PYLT_OBJ_TYPE_OBJ, 0);
    pylt_cclsmethod_register_0_args(state, type, pl_static.str.__new__, &pylt_cls_method_obj_new);
    pylt_obj_type_register(state, type);

    // int
    type = pylt_obj_type_new(state, pl_static.str.int_, PYLT_OBJ_TYPE_INT, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // float
    type = pylt_obj_type_new(state, pl_static.str.float_, PYLT_OBJ_TYPE_FLOAT, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_0_args(state, type, _NS(state, "is_integer"), &pylt_method_int_is_integer);
    pylt_obj_type_register(state, type);

    // bool
    type = pylt_obj_type_new(state, pl_static.str.bool_, PYLT_OBJ_TYPE_BOOL, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 5 ==============

    // str
    type = pylt_obj_type_new(state, pl_static.str.str, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // bytes
    type = pylt_obj_type_new(state, pl_static.str.bytes, PYLT_OBJ_TYPE_BYTES, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // set
    type = pylt_obj_type_new(state, pl_static.str.set, PYLT_OBJ_TYPE_SET, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_1_args(state, type, pl_static.str.add, &pylt_method_set_add);
    pylt_cmethod_register_0_args(state, type, pl_static.str.clear, &pylt_method_set_clear);
    pylt_cmethod_register_0_args(state, type, pl_static.str.copy, &pylt_method_set_copy);
    pylt_cmethod_register_0_args(state, type, pl_static.str.pop, &pylt_method_set_pop);
    pylt_cmethod_register_1_args(state, type, pl_static.str.remove, &pylt_method_set_remove);
    pylt_obj_type_register(state, type);

    // list
    type = pylt_obj_type_new(state, pl_static.str.list, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_1_args(state, type, pl_static.str.append, &pylt_method_list_append);
    pylt_cmethod_register_0_args(state, type, pl_static.str.clear, &pylt_method_list_clear);
    pylt_cmethod_register_0_args(state, type, pl_static.str.copy, &pylt_method_list_copy);
    pylt_cmethod_register_0_args(state, type, pl_static.str.count, &pylt_method_list_count);
    pylt_cmethod_register(state, type, pl_static.str.extend, _NST(state, 2, "self", "object"), NULL, _UINTS(2, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_LIST), &pylt_method_list_extend);
    pylt_cmethod_register_1_args(state, type, pl_static.str.index, &pylt_method_list_index);
    pylt_cmethod_register(state, type, pl_static.str.insert, _NST(state, 3, "self", "index", "object"), NULL, _UINTS(3, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_INT, 0), &pylt_method_list_insert);
    pylt_cmethod_register(state, type, pl_static.str.pop, _NST(state, 2, "self", "index"), _NT(state, 2, 0, pylt_obj_int_new(state, -1)), _UINTS(2, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_INT), &pylt_method_list_pop);
    pylt_cmethod_register_1_args(state, type, pl_static.str.remove, &pylt_method_list_remove);
    pylt_cmethod_register_0_args(state, type, pl_static.str.reverse, &pylt_method_list_reverse);
    pylt_obj_type_register(state, type);

    // tuple
    type = pylt_obj_type_new(state, pl_static.str.tuple, PYLT_OBJ_TYPE_TUPLE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // dict
    type = pylt_obj_type_new(state, pl_static.str.dict, PYLT_OBJ_TYPE_DICT, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 11 ==============

    // module
    type = pylt_obj_type_new(state, pl_static.str.module, PYLT_OBJ_TYPE_MODULE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // function
    type = pylt_obj_type_new(state, pl_static.str.function, PYLT_OBJ_TYPE_FUNCTION, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // cfunction
    type = pylt_obj_type_new(state, pl_static.str.cfunction, PYLT_OBJ_TYPE_CFUNCTION, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // code
    type = pylt_obj_type_new(state, pl_static.str.code, PYLT_OBJ_TYPE_CODE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 15 ==============

    // type
    type = pylt_obj_type_new(state, pl_static.str.type, PYLT_OBJ_TYPE_TYPE, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_0_args(state, type, pl_static.str.mro, &pylt_method_type_mro);
    pylt_cprop_register(state, type, pl_static.str.__base__, &pylt_prop_type_base_get, NULL);
    pylt_cmethod_register(state, type, pl_static.str.__new__, _NST(state, 2, "cls", "object"), NULL, NULL, &pylt_cls_method_type_new);
    pylt_obj_type_register(state, type);

    // iter
    type = pylt_obj_type_new(state, pl_static.str.iterator, PYLT_OBJ_TYPE_ITER, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // none
    type = pylt_obj_type_new(state, pl_static.str.none, PYLT_OBJ_TYPE_NONE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 19 ==============

    // range
    type = pylt_obj_type_new(state, pl_static.str.range, PYLT_OBJ_TYPE_RANGE, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register(state, type, pl_static.str.__new__, _NST(state, 4, "cls", "start", "stop", "step"), _NT(state, 4, PARAM_NODEF, PARAM_NODEF, PARAM_NODEF, pylt_obj_int_new(state, 1)), NULL, &pylt_cls_method_range_new);
    pylt_obj_type_register(state, type);

    state->class_num = PYLT_OBJ_BUILTIN_TYPE_NUM;
}
