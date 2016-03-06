
#ifndef PYLITE_TYPES_FUNC_H
#define PYLITE_TYPES_FUNC_H

#include "object.h"
#include "string.h"
#include "../lib/kvec.h"

typedef struct PyLiteFunctionObject {
    PyLiteObject_HEAD;
    pl_int_t length;                   /* length of parameters */
    pl_int_t minimal;                  /* minimal number of parameters */

    PyLiteStrObject **names;           /* parameters' names */
    PyLiteObject **defaults;           /* default values of option parameters */
    kvec_t(PyLiteObject*) const_val;
    kvec_t(uintptr_t) opcodes;
    PyLiteTable *locals;
    // upvalue
    PyLiteStrObject *doc;
} PyLiteFunctionObject;

PyLiteFunctionObject* pylt_obj_func_new(PyLiteState *state);


typedef struct PyLiteCFunctionObject {
    PyLiteObject_HEAD;
    pl_int_t length;           /* length of parameters */
    pl_int_t minimal;          /* minimal number of parameters */

    PyLiteStrObject **names;   /* parameters' names */
    pl_int_t *type_codes;      /* parameters' type code */
    PyLiteObject **defaults;   /* default values of option parameters */
    PyLiteCFunctionPtr func;   /* the C pointer */
    PyLiteStrObject *doc;
} PyLiteCFunctionObject;


#endif
