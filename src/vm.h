
#ifndef PYLITE_VM_H
#define PYLITE_VM_H

#include "utils.h"
#include "lexer.h"
#include "lib/kvec.h"

enum {
    BC_OPERATOR,
    BC_LOAD,
    BC_SET,
    BC_LOADCONST,
};

enum {
    OP_OR,
    OP_AND,
    OP_NOT,
    OP_IN, OP_NOT_IN, OP_IS, OP_IS_NOT, OP_LT, OP_LE, OP_GT, OP_GE, OP_NE, OP_EQ,
    OP_BITOR,
    OP_BITXOR,
    OP_BITAND,
    OP_LSHIFT, OP_RSHIFT,
    OP_PLUS, OP_MINUS,
    OP_MUL, OP_MATMUL, OP_DIV, OP_FLOORDIV, OP_MOD,
    OP_POS, OP_NEG, OP_BITNOT,
    OP_POW
} OperatorValue;


typedef struct {
    kvec_t(size_t) stack;
} PyLiteVM;

const char* pylt_vm_get_op_name(int op);
int token_to_op_val(uint32_t tk);

#endif
