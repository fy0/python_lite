
#include "bind.h"
#include "common.h"
#include "../state.h"
#include "../types/all.h"
#include "../mods/helper.h"

void pylt_obj_type_register(PyLiteState *state, PyLiteTypeObject* type) {
    if (type->ob_reftype >= kv_size(state->cls_base)) {
        kv_resize(PyLiteTypeObject*, state->cls_base, type->ob_reftype + 10);
    }
    kv_A(state->cls_base, type->ob_reftype) = type;
}

void pylt_bind_all_types_register(PyLiteState *state) {
    PyLiteTypeObject *type;
    kv_resize(PyLiteTypeObject*, state->cls_base, PYLT_OBJ_BUILTIN_TYPE_NUM + 10);

    // object
    type = pylt_obj_type_new(state, _NS(state, "object"), PYLT_OBJ_TYPE_OBJ, 0);
    pylt_cclsmethod_register_0_args(state, type, _NS(state, "__new__"), &pylt_cls_method_obj_new);
    pylt_obj_type_register(state, type);

    // int
    type = pylt_obj_type_new(state, _NS(state, "int"), PYLT_OBJ_TYPE_INT, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // float
    type = pylt_obj_type_new(state, _NS(state, "float"), PYLT_OBJ_TYPE_FLOAT, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_0_args(state, type, _NS(state, "is_integer"), &pylt_method_int_is_integer);
    pylt_obj_type_register(state, type);

    // bool
    type = pylt_obj_type_new(state, _NS(state, "bool"), PYLT_OBJ_TYPE_BOOL, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 5 ==============

    // str
    type = pylt_obj_type_new(state, _NS(state, "str"), PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // bytes
    type = pylt_obj_type_new(state, _NS(state, "bytes"), PYLT_OBJ_TYPE_BYTES, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // set
    type = pylt_obj_type_new(state, _NS(state, "set"), PYLT_OBJ_TYPE_SET, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_1_args(state, type, _NS(state, "add"), &pylt_method_set_add);
    pylt_cmethod_register_0_args(state, type, _NS(state, "clear"), &pylt_method_set_clear);
    pylt_cmethod_register_0_args(state, type, _NS(state, "copy"), &pylt_method_set_copy);
    pylt_cmethod_register_0_args(state, type, _NS(state, "pop"), &pylt_method_set_pop);
    pylt_cmethod_register_1_args(state, type, _NS(state, "remove"), &pylt_method_set_remove);
    pylt_obj_type_register(state, type);

    // list
    type = pylt_obj_type_new(state, _NS(state, "list"), PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_OBJ);
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
    type = pylt_obj_type_new(state, _NS(state, "tuple"), PYLT_OBJ_TYPE_TUPLE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // dict
    type = pylt_obj_type_new(state, _NS(state, "dict"), PYLT_OBJ_TYPE_DICT, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 11 ==============

    // module
    type = pylt_obj_type_new(state, _NS(state, "module"), PYLT_OBJ_TYPE_MODULE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // function
    type = pylt_obj_type_new(state, _NS(state, "function"), PYLT_OBJ_TYPE_FUNCTION, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // cfunction
    type = pylt_obj_type_new(state, _NS(state, "cfunction"), PYLT_OBJ_TYPE_CFUNCTION, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // code
    type = pylt_obj_type_new(state, _NS(state, "code"), PYLT_OBJ_TYPE_CODE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // ============== 15 ==============

    // type
    type = pylt_obj_type_new(state, _NS(state, "type"), PYLT_OBJ_TYPE_TYPE, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_0_args(state, type, _NS(state, "mro"), &pylt_method_type_mro);
    pylt_cprop_register(state, type, _NS(state, "__base__"), &pylt_prop_type_base_get, NULL);
    pylt_cmethod_register(state, type, _NS(state, "__new__"), _NST(state, 2, "cls", "object"), NULL, NULL, &pylt_cls_method_type_new);
    pylt_obj_type_register(state, type);

    // iter
    type = pylt_obj_type_new(state, _NS(state, "iterator"), PYLT_OBJ_TYPE_ITER, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    // none
    type = pylt_obj_type_new(state, _NS(state, "none"), PYLT_OBJ_TYPE_NONE, PYLT_OBJ_TYPE_OBJ);
    pylt_obj_type_register(state, type);

    state->class_num = PYLT_OBJ_BUILTIN_TYPE_NUM;
}
