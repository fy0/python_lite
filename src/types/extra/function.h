
#ifndef PYLITE_TYPES_FUNC_H
#define PYLITE_TYPES_FUNC_H

#include "../object.h"
#include "../common/string.h"
#include "../common/tuple.h"
#include "../common/dict.h"
#include "code.h"

typedef struct PyLiteFunctionInfo {
    pl_int_t length;           /* length of parameters */
    pl_int_t minimal;          /* minimal number of parameters */

    PyLiteStrObject *name;
	PyLiteTupleObject *params;    /* parameters' names */
    pl_int_t *type_codes;         /* parameters' type code */
    PyLiteTupleObject *defaults;  /* default values of parameters */
    PyLiteStrObject *doc;
    PyLiteDictObject *scope;
} PyLiteFunctionInfo;

typedef struct PyLiteFunctionObject {
    PyLiteObject_HEAD;
    PyLiteObject *ob_owner; /* owner of object */
    PyLiteFunctionInfo info;
    PyLiteCodeObject code;
} PyLiteFunctionObject;

typedef struct PyLiteCFunctionObject {
    PyLiteObject_HEAD;
    PyLiteObject *ob_owner; /* owner of object */
    PyLiteFunctionInfo info;
    PyLiteCFunctionPtr code;   /* the C pointer */
} PyLiteCFunctionObject;


PyLiteFunctionObject* pylt_obj_func_new(PyLiteInterpreter *I, PyLiteCodeObject *code);
PyLiteFunctionObject* pylt_obj_func_new_ex(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteListObject *params, PyLiteCodeObject *code, PyLiteDictObject *defaults, PyLiteStrObject *args, PyLiteStrObject *kwargs);
PyLiteFunctionInfo* pylt_obj_func_get_info(PyLiteInterpreter *I, PyLiteObject *func);

PyLiteCFunctionObject* pylt_obj_cfunc_new(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, pl_uint_t *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_obj_cfunc_new_no_args(PyLiteInterpreter *I, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);

#endif
