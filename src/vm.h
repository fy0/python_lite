
#ifndef PYLITE_VM_H
#define PYLITE_VM_H

#include "utils.h"
#include "lib/kvec.h"

enum {
    BC_OPERATOR,
    BC_LOAD,
    BC_SET,
    BC_LOADCONST,
};

typedef struct {
    kvec_t(size_t) stack;
} PyLiteVM;

#endif
