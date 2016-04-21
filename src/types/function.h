
#ifndef PYLITE_TYPES_FUNC_H
#define PYLITE_TYPES_FUNC_H

#include "object.h"
#include "string.h"
#include "../lib/kvec.h"
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

PyLiteFunctionObject* pylt_obj_func_new(PyLiteState *state, PyLiteCodeSnippetObject *code);


typedef struct PyLiteCFunctionObject {
    PyLiteObject_HEAD;
    PyLiteFunctionInfo info;
    PyLiteCFunctionPtr code;   /* the C pointer */
} PyLiteCFunctionObject;


PyLiteFunctionInfo* pylt_obj_func_get_info(PyLiteState *state, PyLiteObject *func);


#endif
