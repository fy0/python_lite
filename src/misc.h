
#ifndef PYLITE_MISC_H
#define PYLITE_MISC_H

#include "types/all.h"
#include "intp.h"

struct PyLiteStaticObjectsType {
    struct {
        PyLiteStrObject *__init__;
        PyLiteStrObject *__new__;
        PyLiteStrObject *__call__;
        PyLiteStrObject *__base__;
        PyLiteStrObject *__import__;

        PyLiteStrObject *__add__;
        PyLiteStrObject *__sub__;
        PyLiteStrObject *__mul__;
        PyLiteStrObject *__div__;
        PyLiteStrObject *__floordiv__;
        PyLiteStrObject *__pos__;
        PyLiteStrObject *__neg__;
        PyLiteStrObject *__pow__;

        PyLiteStrObject *__lshift__;
        PyLiteStrObject *__rshift__;

        PyLiteStrObject *__hash__;
        PyLiteStrObject *__iter__;
        PyLiteStrObject *__cmp__;
        PyLiteStrObject *__eq__;
        PyLiteStrObject *__setattr__;
        PyLiteStrObject *__getattr__;
        PyLiteStrObject *__setitem__;
        PyLiteStrObject *__getitem__;

        PyLiteStrObject *id;
        PyLiteStrObject *dir;
        PyLiteStrObject *len;
        PyLiteStrObject *hash;
        PyLiteStrObject *iter;
        PyLiteStrObject *repr;
        PyLiteStrObject *super;
        PyLiteStrObject *isinstance;
        PyLiteStrObject *print;
        PyLiteStrObject *pow;
        PyLiteStrObject *setattr;
        PyLiteStrObject *getattr;
        PyLiteStrObject *range;

        PyLiteStrObject *object;
        PyLiteStrObject *int_;
        PyLiteStrObject *float_;
        PyLiteStrObject *bool_;
        PyLiteStrObject *str;
        PyLiteStrObject *bytes;
        PyLiteStrObject *set;
        PyLiteStrObject *list;
        PyLiteStrObject *tuple;
        PyLiteStrObject *dict;

        PyLiteStrObject *module;
        PyLiteStrObject *function;
        PyLiteStrObject *cfunction;
        PyLiteStrObject *code;
        PyLiteStrObject *type;
        PyLiteStrObject *iterator;
        PyLiteStrObject *property_;
        PyLiteStrObject *NoneType;

        // object
        PyLiteStrObject *mro;

        // str
        PyLiteStrObject *join;
        PyLiteStrObject *sub;
        PyLiteStrObject *start;
        PyLiteStrObject *end;

        // int
        PyLiteStrObject *is_integer;

        // set
        PyLiteStrObject *add;
        PyLiteStrObject *clear;
        PyLiteStrObject *copy;
        PyLiteStrObject *pop;
        PyLiteStrObject *remove;

        // list
        PyLiteStrObject *append;
        PyLiteStrObject *count;
        PyLiteStrObject *index;
        PyLiteStrObject *extend;
        PyLiteStrObject *insert;
        PyLiteStrObject *reverse;

        // prop
        PyLiteStrObject *fget;
        PyLiteStrObject *fset;

        PyLiteStrObject *None;
        PyLiteStrObject *True;
        PyLiteStrObject *False;

        PyLiteStrObject *math;
        PyLiteStrObject *builtins;

        PyLiteStrObject *cls;
        PyLiteStrObject *self;
        PyLiteStrObject *args;
        PyLiteStrObject *kwargs;
        PyLiteStrObject *param1;
        PyLiteStrObject *param2;
        PyLiteStrObject *param3;
        PyLiteStrObject *param4;

        PyLiteStrObject *x;
        PyLiteStrObject *y;

        PyLiteStrObject *BaseException;
        PyLiteStrObject *SystemExit;
        PyLiteStrObject *KeyboardInterrupt;
        PyLiteStrObject *GeneratorExit;
        PyLiteStrObject *Exception;
        PyLiteStrObject *StopIteration;
        PyLiteStrObject *StopAsyncIteration;
        PyLiteStrObject *ArithmeticError;
        PyLiteStrObject *FloatingPointError;
        PyLiteStrObject *OverflowError;
        PyLiteStrObject *ZeroDivisionError;
        PyLiteStrObject *AssertionError;
        PyLiteStrObject *AttributeError;
        PyLiteStrObject *BufferError;
        PyLiteStrObject *EOFError;
        PyLiteStrObject *ImportError;
        PyLiteStrObject *LookupError;
        PyLiteStrObject *IndexError;
        PyLiteStrObject *KeyError;
        PyLiteStrObject *MemoryError;
        PyLiteStrObject *NameError;
        PyLiteStrObject *UnboundLocalError;
        PyLiteStrObject *OSError;
        PyLiteStrObject *BlockingIOError;
        PyLiteStrObject *ChildProcessError;
        PyLiteStrObject *ConnectionError;
        PyLiteStrObject *BrokenPipeError;
        PyLiteStrObject *ConnectionAbortedError;
        PyLiteStrObject *ConnectionRefusedError;
        PyLiteStrObject *ConnectionResetError;
        PyLiteStrObject *FileExistsError;
        PyLiteStrObject *FileNotFoundError;
        PyLiteStrObject *InterruptedError;
        PyLiteStrObject *IsADirectoryError;
        PyLiteStrObject *NotADirectoryError;
        PyLiteStrObject *PermissionError;
        PyLiteStrObject *ProcessLookupError;
        PyLiteStrObject *TimeoutError;
        PyLiteStrObject *ReferenceError;
        PyLiteStrObject *RuntimeError;
        PyLiteStrObject *NotImplementedError;
        PyLiteStrObject *RecursionError;
        PyLiteStrObject *SyntaxError;
        PyLiteStrObject *IndentationError;
        PyLiteStrObject *TabError;
        PyLiteStrObject *SystemError;
        PyLiteStrObject *TypeError;
        PyLiteStrObject *ValueError;
        PyLiteStrObject *UnicodeError;
        PyLiteStrObject *UnicodeDecodeError;
        PyLiteStrObject *UnicodeEncodeError;
        PyLiteStrObject *UnicodeTranslateError;
        PyLiteStrObject *Warning;
        PyLiteStrObject *DeprecationWarning;
        PyLiteStrObject *PendingDeprecationWarning;
        PyLiteStrObject *RuntimeWarning;
        PyLiteStrObject *SyntaxWarning;
        PyLiteStrObject *UserWarning;
        PyLiteStrObject *FutureWarning;
        PyLiteStrObject *ImportWarning;
        PyLiteStrObject *UnicodeWarning;
        PyLiteStrObject *BytesWarning;
        PyLiteStrObject *ResourceWarning;

        // template
        PyLiteStrObject *TMPL_OBJECT_TO_STR;
        PyLiteStrObject *TMPL_CLASS_TO_STR;
        PyLiteStrObject *TMPL_MODULE_TO_STR;
        PyLiteStrObject *TMPL_FUNCTION_TO_STR;
        PyLiteStrObject *TMPL_CFUNCTION_TO_STR;

        PyLiteStrObject *TMPL_EMPTY_STR;
        PyLiteStrObject *TMPL_EMPTY_SET;
        PyLiteStrObject *TMPL_EMPTY_LIST;
        PyLiteStrObject *TMPL_EMPTY_TUPLE;
        PyLiteStrObject *TMPL_EMPTY_DICT;
    } str;
} pl_static;

void pylt_misc_static_objs_init(PyLiteInterpreter *I);


#endif

