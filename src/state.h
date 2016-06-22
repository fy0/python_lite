
#ifndef PYLITE_STATE_H
#define PYLITE_STATE_H

#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include "gc.h"
#include "lib/kvec.h"
#include "types/set.h"
#include "types/type.h"

typedef struct PyLiteState {
    PyLiteVM vm;
    PyLiteGC gc;
    LexState *lexer;
    ParserState *parser;

    kvec_t(PyLiteTypeObject*) cls_base;
    PyLiteSetObject *cache_str;
    PyLiteSetObject *cache_bytes;
    PyLiteDictObject *modules;
    pl_int_t class_num;
    int error_code;
} PyLiteState;


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
        PyLiteStrObject *none;

        // object
        PyLiteStrObject *mro;

        // str
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
		PyLiteStrObject *TMPL_EMPTY_SET;
	} str;
} pl_static;

PyLiteState* pylt_state_new();
void pylt_state_free(PyLiteState *state);

void pylt_state_init(PyLiteState *state);
void pylt_state_finalize(PyLiteState *state);
void pylt_state_run(PyLiteState *state);
void pylt_state_load_stream(PyLiteState *state, StringStream *ss);
//void pylt_state_err(PyLiteState *state);

void pylt_utils_static_objs_init(PyLiteState *state);

#endif
