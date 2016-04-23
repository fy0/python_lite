
#ifndef PYLITE_TYPES_FUNC_H
#define PYLITE_TYPES_FUNC_H

#include "object.h"
#include "string.h"
#include "tuple.h"
#include "codesnippet.h"

typedef struct PyLiteFunctionInfo {
    pl_int_t length;           /* length of parameters */
    pl_int_t minimal;          /* minimal number of parameters */

    PyLiteStrObject *name;
    PyLiteStrObject **params;  /* parameters' names */
    pl_int_t *type_codes;      /* parameters' type code */
    PyLiteObject **defaults;   /* default values of option parameters */
    PyLiteStrObject *doc;
} PyLiteFunctionInfo;

typedef struct PyLiteFunctionObject {
    PyLiteObject_HEAD;
    PyLiteFunctionInfo info;
    PyLiteCodeSnippetObject code;
} PyLiteFunctionObject;


typedef struct PyLiteCFunctionObject {
    PyLiteObject_HEAD;
    PyLiteFunctionInfo info;
    PyLiteCFunctionPtr code;   /* the C pointer */
} PyLiteCFunctionObject;


PyLiteFunctionObject* pylt_obj_func_new(PyLiteState *state, PyLiteCodeSnippetObject *code);
PyLiteFunctionInfo* pylt_obj_func_get_info(PyLiteState *state, PyLiteObject *func);

PyLiteCFunctionObject* pylt_obj_cfunc_new(PyLiteState *state, PyLiteStrObject *name, PyLiteTupleObject *param_names, PyLiteTupleObject *defaults, int *types, PyLiteCFunctionPtr cfunc);
PyLiteCFunctionObject* pylt_obj_cfunc_new_no_args(PyLiteState *state, PyLiteStrObject *name, PyLiteCFunctionPtr cfunc);

#endif
