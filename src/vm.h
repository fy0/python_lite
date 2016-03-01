
#ifndef PYLITE_VM_H
#define PYLITE_VM_H

#include "utils.h"
#include "lib/kvec.h"

enum {
    BC_OPERATOR,   // 运算符
    BC_SET_VAL,    // 赋值
    BC_LOAD_VAL,   // 载入变量（入栈）
    BC_LOADCONST,  // 载入常量
    BC_NEW_OBJ,    // 新建对象
    BC_CALL,       // 函数调用
    BC_PRINT,      // 测试输出
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


typedef struct PyLiteVM{
    kvec_t(uintptr_t) stack;
} PyLiteVM;

struct PyLiteState;

const char* pylt_vm_get_op_name(int op);
int token_to_op_val(uint32_t tk);
void pylt_vm_init(PyLiteVM* vm);
void pylt_vm_run(struct PyLiteState* state);

#endif
