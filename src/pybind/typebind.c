
#include "../bind.h"
#include "../intp.h"
#include "../types/all.h"
#include "common.h"
#include "extra.h"

#define EXCEPTION_REGISTER(name, the_base) \
    type = pylt_obj_type_new(I, pl_static.str.name, the_base, NULL); \
    pylt_obj_type_register(I, type);

void pylt_bind_exceptions(PyLiteInterpreter *I) {
    PyLiteTypeObject *type;
    pl_uint32_t base, base2;

    EXCEPTION_REGISTER(SystemExit, PYLT_OBJ_TYPE_BASE_EXCEPTION);
    EXCEPTION_REGISTER(KeyboardInterrupt, PYLT_OBJ_TYPE_BASE_EXCEPTION);
    EXCEPTION_REGISTER(GeneratorExit, PYLT_OBJ_TYPE_BASE_EXCEPTION);

    EXCEPTION_REGISTER(Exception, PYLT_OBJ_TYPE_BASE_EXCEPTION);
    base = type->ob_base;
    EXCEPTION_REGISTER(StopIteration, base);
    EXCEPTION_REGISTER(StopAsyncIteration, base);
    EXCEPTION_REGISTER(AssertionError, base);
    EXCEPTION_REGISTER(AttributeError, base);
    EXCEPTION_REGISTER(BufferError, base);
    EXCEPTION_REGISTER(EOFError, base);
    EXCEPTION_REGISTER(ImportError, base);
    EXCEPTION_REGISTER(MemoryError, base);
    EXCEPTION_REGISTER(NameError, base);
    EXCEPTION_REGISTER(ReferenceError, base);
    EXCEPTION_REGISTER(SystemError, base);
    EXCEPTION_REGISTER(TypeError, base);
    EXCEPTION_REGISTER(ValueError, base);

    EXCEPTION_REGISTER(ArithmeticError, base);
    base2 = type->ob_base;
    EXCEPTION_REGISTER(FloatingPointError, base2);
    EXCEPTION_REGISTER(OverflowError, base2);
    EXCEPTION_REGISTER(ZeroDivisionError, base2);

    EXCEPTION_REGISTER(LookupError, base);
    base2 = type->ob_base;
    EXCEPTION_REGISTER(IndexError, base2);
    EXCEPTION_REGISTER(KeyError, base2);

    EXCEPTION_REGISTER(OSError, base);
    base2 = type->ob_base;
    EXCEPTION_REGISTER(BlockingIOError, base2);
    EXCEPTION_REGISTER(ChildProcessError, base2);
    EXCEPTION_REGISTER(FileExistsError, base2);
    EXCEPTION_REGISTER(FileNotFoundError, base2);
    EXCEPTION_REGISTER(InterruptedError, base2);
    EXCEPTION_REGISTER(IsADirectoryError, base2);
    EXCEPTION_REGISTER(NotADirectoryError, base2);
    EXCEPTION_REGISTER(PermissionError, base2);
    EXCEPTION_REGISTER(ProcessLookupError, base2);
    EXCEPTION_REGISTER(TimeoutError, base2);

    EXCEPTION_REGISTER(RuntimeError, base);
    base2 = type->ob_base;
    EXCEPTION_REGISTER(NotImplementedError, base2);
    EXCEPTION_REGISTER(RecursionError, base2);

    EXCEPTION_REGISTER(SyntaxError, base);
    base2 = type->ob_base;
    EXCEPTION_REGISTER(IndentationError, base2);

    EXCEPTION_REGISTER(Warning, base);
    base2 = type->ob_base;
    EXCEPTION_REGISTER(DeprecationWarning, base2);
    EXCEPTION_REGISTER(PendingDeprecationWarning, base2);
    EXCEPTION_REGISTER(RuntimeWarning, base2);
    EXCEPTION_REGISTER(SyntaxWarning, base2);
    EXCEPTION_REGISTER(UserWarning, base2);
    EXCEPTION_REGISTER(FutureWarning, base2);
    EXCEPTION_REGISTER(ImportWarning, base2);
    EXCEPTION_REGISTER(UnicodeWarning, base2);
    EXCEPTION_REGISTER(BytesWarning, base2);
    EXCEPTION_REGISTER(ResourceWarning, base2);
}


void pylt_bind_all_types_register(PyLiteInterpreter *I) {
    PyLiteTypeObject *type;
    kv_resize(PyLiteTypeObject*, I->cls_base, PYLT_OBJ_BUILTIN_TYPE_NUM + 20);

	// ============== 0 ==============

    // object
    type = pylt_obj_type_new_with_type(I, pl_static.str.object, PYLT_OBJ_TYPE_OBJ, 0);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_obj_new);
    pylt_cmethod_register_0_args(I, type, _S(__str__), &pylt_method_obj_repr);
    pylt_cmethod_register_0_args(I, type, _S(__repr__), &pylt_method_obj_str);
    pylt_obj_type_register(I, type);

    // int
    type = pylt_obj_type_new_with_type(I, pl_static.str.int_, PYLT_OBJ_TYPE_INT, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_int_new);
    pylt_obj_type_register(I, type);

    // float
    type = pylt_obj_type_new_with_type(I, pl_static.str.float_, PYLT_OBJ_TYPE_FLOAT, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_float_new);
    pylt_cmethod_register_0_args(I, type, _NS(I, "is_integer"), &pylt_method_int_is_integer);
    pylt_obj_type_register(I, type);

    // bool
    type = pylt_obj_type_new_with_type(I, pl_static.str.bool_, PYLT_OBJ_TYPE_BOOL, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register(I, type, _S(__new__), _NT(I, 2, _S(cls), _S(object)), _NT(I, 2, castobj(&PyLiteParamUndefined), &PyLiteFalse), NULL, &pylt_cls_method_bool_new);
    pylt_obj_type_register(I, type);

    // ============== 5 ==============

    // str
    type = pylt_obj_type_new_with_type(I, pl_static.str.str, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_str_new);
    pylt_cmethod_register(I, type, _S(index), _NT(I, 4, _S(self), _S(sub), _S(start), _S(end)), _NT(I, 4, castobj(&PyLiteParamUndefined), castobj(&PyLiteParamUndefined), _NI(0), _NI(-1)), _UINTS(4, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_STR, PYLT_OBJ_TYPE_INT, PYLT_OBJ_TYPE_INT), &pylt_method_str_index);
    pylt_cmethod_register(I, type, _S(join), _NST(I, 2, "self", "sequence"), NULL, _UINTS(2, PYLT_OBJ_TYPE_STR, castobj(&PyLiteParamUndefined)), &pylt_method_str_join);
    pylt_obj_type_register(I, type);

    // bytes
    type = pylt_obj_type_new_with_type(I, pl_static.str.bytes, PYLT_OBJ_TYPE_BYTES, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_bytes_new);
    pylt_obj_type_register(I, type);

    // set
    type = pylt_obj_type_new_with_type(I, pl_static.str.set, PYLT_OBJ_TYPE_SET, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register(I, type, _S(__new__), _NT(I, 2, _S(cls), _S(object)), _NT(I, 2, castobj(&PyLiteParamUndefined), pylt_obj_tuple_new(I, 0)), NULL, &pylt_cls_method_set_new);
    pylt_cmethod_register_1_args(I, type, pl_static.str.add, &pylt_method_set_add);
    pylt_cmethod_register_0_args(I, type, pl_static.str.clear, &pylt_method_set_clear);
    pylt_cmethod_register_0_args(I, type, pl_static.str.copy, &pylt_method_set_copy);
    pylt_cmethod_register_0_args(I, type, pl_static.str.pop, &pylt_method_set_pop);
    pylt_cmethod_register_1_args(I, type, pl_static.str.remove, &pylt_method_set_remove);
    pylt_obj_type_register(I, type);

    // list
    type = pylt_obj_type_new_with_type(I, pl_static.str.list, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register(I, type, _S(__new__), _NST(I, 2, "cls", "obj"), _NT(I, 2, castobj(&PyLiteParamUndefined), pylt_obj_tuple_new(I, 0)), NULL, &pylt_cls_method_list_new);
    pylt_cmethod_register_1_args(I, type, pl_static.str.append, &pylt_method_list_append);
    pylt_cmethod_register_0_args(I, type, pl_static.str.clear, &pylt_method_list_clear);
    pylt_cmethod_register_0_args(I, type, pl_static.str.copy, &pylt_method_list_copy);
    pylt_cmethod_register_0_args(I, type, pl_static.str.count, &pylt_method_list_count);
    pylt_cmethod_register(I, type, pl_static.str.extend, _NST(I, 2, "self", "object"), NULL, _UINTS(2, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_LIST), &pylt_method_list_extend);
    pylt_cmethod_register_1_args(I, type, pl_static.str.index, &pylt_method_list_index);
    pylt_cmethod_register(I, type, pl_static.str.insert, _NST(I, 3, "self", "index", "object"), NULL, _UINTS(3, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_INT, 0), &pylt_method_list_insert);
    pylt_cmethod_register(I, type, pl_static.str.pop, _NST(I, 2, "self", "index"), _NT(I, 2, 0, pylt_obj_int_new(I, -1)), _UINTS(2, PYLT_OBJ_TYPE_LIST, PYLT_OBJ_TYPE_INT), &pylt_method_list_pop);
    pylt_cmethod_register_1_args(I, type, pl_static.str.remove, &pylt_method_list_remove);
    pylt_cmethod_register_0_args(I, type, pl_static.str.reverse, &pylt_method_list_reverse);
    pylt_obj_type_register(I, type);

    // tuple
    type = pylt_obj_type_new_with_type(I, pl_static.str.tuple, PYLT_OBJ_TYPE_TUPLE, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register(I, type, _S(__new__), _NT(I, 2, _S(cls), _S(object)), _NT(I, 2, castobj(&PyLiteParamUndefined), pylt_obj_tuple_new(I, 0)), NULL, &pylt_cls_method_tuple_new);
    pylt_obj_type_register(I, type);

    // dict
    type = pylt_obj_type_new_with_type(I, pl_static.str.dict, PYLT_OBJ_TYPE_DICT, PYLT_OBJ_TYPE_OBJ);
	pylt_cclsmethod_register(I, type, _S(__new__), _NT(I, 2, _S(cls), _S(object)), _NT(I, 2, castobj(&PyLiteParamUndefined), castobj(&PyLiteParamKwargs)), NULL, &pylt_cls_method_dict_new);
    pylt_obj_type_register(I, type);

	// unusual
	type = pylt_obj_type_new_with_type(I, _S(unusual), PYLT_OBJ_TYPE_UNUSUAL, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_meaningless_new);
	pylt_obj_type_register(I, type);

    // ============== 12 ==============

    // module
    type = pylt_obj_type_new_with_type(I, pl_static.str.module, PYLT_OBJ_TYPE_MODULE, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_meaningless_new);
	type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // function
    type = pylt_obj_type_new_with_type(I, pl_static.str.function, PYLT_OBJ_TYPE_FUNCTION, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_meaningless_new);
	pylt_cprop_register(I, type, _S(__defaults__), &pylt_prop_function_defaults_get, NULL);
	pylt_cprop_register(I, type, _S(__parameters__), &pylt_prop_function_parameters_get, NULL);
	type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // cfunction
	type = pylt_obj_type_new_with_type(I, pl_static.str.cfunction, PYLT_OBJ_TYPE_CFUNCTION, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_meaningless_new);
	pylt_cprop_register(I, type, _S(__defaults__), &pylt_prop_function_defaults_get, NULL);
	pylt_cprop_register(I, type, _S(__parameters__), &pylt_prop_function_parameters_get, NULL);
	type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // code
	type = pylt_obj_type_new_with_type(I, pl_static.str.code, PYLT_OBJ_TYPE_CODE, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_meaningless_new);
    type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // ============== 16 ==============

    // type
    type = pylt_obj_type_new_with_type(I, pl_static.str.type, PYLT_OBJ_TYPE_TYPE, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register_0_args(I, type, pl_static.str.mro, &pylt_method_type_mro);
    pylt_cprop_register(I, type, pl_static.str.__base__, &pylt_prop_type_base_get, NULL);
    pylt_cmethod_register(I, type, pl_static.str.__new__, _NST(I, 2, "cls", "object"), NULL, NULL, &pylt_cls_method_type_new);
    type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // prop
    type = pylt_obj_type_new_with_type(I, pl_static.str.property_, PYLT_OBJ_TYPE_PROP, PYLT_OBJ_TYPE_OBJ);
    type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // none
	type = pylt_obj_type_new_with_type(I, pl_static.str.NoneType, PYLT_OBJ_TYPE_NONE, PYLT_OBJ_TYPE_OBJ);
    pylt_cclsmethod_register_0_args(I, type, _S(__new__), &pylt_cls_method_meaningless_new);
    type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // iter
    type = pylt_obj_type_new_with_type(I, pl_static.str.iterator, PYLT_OBJ_TYPE_ITER, PYLT_OBJ_TYPE_OBJ);
    type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // ============== 20 ==============

    // range
    type = pylt_obj_type_new_with_type(I, pl_static.str.range, PYLT_OBJ_TYPE_RANGE, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register(I, type, _S(__new__), _NST(I, 4, "cls", "start", "stop", "step"), _NT(I, 4, castobj(&PyLiteParamUndefined), castobj(&PyLiteParamUndefined), &PyLiteNone, pylt_obj_int_new(I, 1)), NULL, &pylt_cls_method_range_new);
    type->allow_inherit = false;
    pylt_obj_type_register(I, type);

    // exception
    type = pylt_obj_type_new_with_type(I, pl_static.str.BaseException, PYLT_OBJ_TYPE_BASE_EXCEPTION, PYLT_OBJ_TYPE_OBJ);
    pylt_cmethod_register(I, type, _S(__new__), _NT(I, 2, _S(cls), _S(args)), _NT(I, 2, castobj(&PyLiteParamUndefined), castobj(&PyLiteParamArgs)), NULL, &pylt_cls_method_base_exception_new);
    pylt_cprop_register(I, type, _S(args), &pylt_prop_base_exception_args_get, NULL);
    pylt_obj_type_register(I, type);

    I->class_num = PYLT_OBJ_BUILTIN_TYPE_NUM;

    pylt_bind_exceptions(I);
}
