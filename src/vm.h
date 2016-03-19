
#ifndef PYLITE_VM_H
#define PYLITE_VM_H

#include "utils.h"
#include "lib/kvec.h"
#include "types/all.h"

enum {
    BC_OPERATOR,       // 运算符
    BC_SET_VAL,        // 赋值
    BC_LOAD_VAL,       // 载入变量（入栈）
    BC_LOADCONST,      // 载入常量
    BC_NEW_OBJ,        // 新建对象
    BC_NEW_OBJ_EXTRA,  // 新建对象（带1额外参数）
    BC_CALL,           // 函数调用
    BC_CALL_END,       // 函数结束
    BC_PRINT,          // 测试输出
    BC_TEST,           // 真值测试
    BC_JMP,            // 跳转
    BC_JMP_BACK,       // 前向跳转
    BC_POP,            // 弹出一个值
    BC_FORITER,        // For循环
    BC_DEL_FORCE,      // 强制释放
    BC_GET_ITEM,       // 取下标
    BC_SET_ITEM,       // 存入下标

    BC_FAKE_BREAK,     // 伪指令，break
    BC_FAKE_CONTINUE,  // 伪指令，continue
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


typedef struct PyLiteFuncCall {
    PyLiteFunctionObject *func;
    pl_uint_t code_pointer;
    kvec_t(PyLiteTable*) var_tables;
} PyLiteFuncCall;

typedef struct PyLiteVM {
    kvec_t(uintptr_t) stack;
    kvec_t(PyLiteFuncCall) calls;
} PyLiteVM;

struct PyLiteState;

void pylt_vm_call_func(PyLiteState* state, PyLiteFunctionObject *func);

const char* pylt_vm_get_op_name(int op);
int token_to_op_val(uint32_t tk);
int token_de_to_op_val(uint32_t tk);

void pylt_vm_init(struct PyLiteState *state, PyLiteVM *vm);
void pylt_vm_run(PyLiteState* state, PyLiteFunctionObject *func);

#endif
