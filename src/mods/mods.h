
#ifndef PYLITE_MODS_H
#define PYLITE_MODS_H

#include "../types/all.h"

typedef struct PyLiteCFunction {
    pl_int_t length;           /* length of parameters */
    pl_int_t minimal;          /* minimal number of parameters */

    PyLiteObject **names;      /* parameters' names */
    PyLiteObject **defaults;   /* default values of option parameters */
    PyLiteCFunctionPtr func;   /* the C pointer */
} PyLiteCFunction;

#endif
