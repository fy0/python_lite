
#ifndef PYLITE_VM_H
#define PYLITE_VM_H

#include "config.h"
#include "lib/kvec.h"
#include "types/all.h"
#include "api.h"

enum {
    BC_OPERATOR,       // 运算符
    BC_SET_VAL,        // 赋值
    BC_LOAD_VAL,       // 载入变量（入栈）
    BC_LOADNONE,       // 载入 None
    BC_LOADCONST,      // 载入常量
    BC_LOADLOCALS,     // 载入本地变量空间
    BC_NEW_OBJ,        // 新建对象
    BC_CALL,           // 函数调用
    BC_RET,            // 函数结束
    BC_HALT,           // 程序结束
    BC_PRINT,          // 测试输出
    BC_TEST,           // 真值测试
    BC_JMP,            // 跳转
    BC_JMP_BACK,       // 前向跳转
    BC_POP,            // 弹出一个值
    BC_FORITER,        // For循环
    BC_DEL_FORCE,      // 强制释放
    BC_GET_ITEM,       // 取下标
    BC_SET_ITEM,       // 存入下标
    BC_GET_ATTR,       // 取属性
    BC_SET_ATTR,       // 存入属性
    BC_ASSERT,         // 真值断言

    BC_IMPORT_NAME,    // 导入模块

    BC_GET_ITEM_EX,    // 取下标
    BC_LOAD_VAL_EX,    // 载入变量
    BC_GET_ATTR_EX,    // 取属性

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

typedef struct PyLiteFrame {
    PyLiteFunctionObject *func;
    PyLiteCodeObject *code;
    pl_uint_t code_pointer_slot;
	pl_bool_t halt_when_ret;
    kvec_t(PyLiteDictObject*) var_tables;
} PyLiteFrame;

typedef struct PyLiteVM {
    kvec_t(uintptr_t) stack;
    kvec_t(PyLiteFrame) frames;
    PyLiteModuleObject *builtins;
} PyLiteVM;

#define kv_pushptr(v, x) kv_push(uintptr_t, (v), (uintptr_t)(x))

void pylt_vm_load_func(PyLiteInterpreter *I, PyLiteFunctionObject *func);
void pylt_vm_load_code(PyLiteInterpreter *I, PyLiteCodeObject *code);

const char* pylt_vm_get_op_name(int op);
int token_to_op_val(uint32_t tk);
int token_de_to_op_val(uint32_t tk);

PyLiteFrame* pylt_vm_curframe(PyLiteInterpreter *I);

void pylt_vm_init(PyLiteInterpreter *I, PyLiteVM *vm);
void pylt_vm_finalize(PyLiteInterpreter *I);
void pylt_vm_run(PyLiteInterpreter *I, PyLiteCodeObject *code);

#endif
