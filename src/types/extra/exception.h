
#ifndef PYLITE_TYPES_EXCEPTION_H
#define PYLITE_TYPES_EXCEPTION_H

#include "../object.h"
#include "../custom.h"
#include "../common/string.h"
#include "../common/tuple.h"

typedef struct PyLiteBaseExceptionObject {
    PyLiteObject_HEAD;
    PyLiteTupleObject *args;
} PyLiteBaseExceptionObject;

typedef struct PyLiteExceptionObject {
    PyLiteCustomObject_HEAD;
} PyLiteExceptionObject;

#define PYLITE_EXCEPTION_DEFINE(x) \
    typedef struct PyLiteE##x##Object { \
        PyLiteCustomObject_HEAD; \
    } PyLiteE##x##Object;

// base exception
PYLITE_EXCEPTION_DEFINE(SystemExit);
PYLITE_EXCEPTION_DEFINE(KeyboardInterrupt);
PYLITE_EXCEPTION_DEFINE(GeneratorExit);

// --> exception
PYLITE_EXCEPTION_DEFINE(StopIteration);
PYLITE_EXCEPTION_DEFINE(StopAsyncIteration);
PYLITE_EXCEPTION_DEFINE(ArithmeticError);
PYLITE_EXCEPTION_DEFINE(AssertionError);
PYLITE_EXCEPTION_DEFINE(AttributeError);
PYLITE_EXCEPTION_DEFINE(BufferError);
PYLITE_EXCEPTION_DEFINE(EOFError);
PYLITE_EXCEPTION_DEFINE(ImportError);
PYLITE_EXCEPTION_DEFINE(LookupError);
PYLITE_EXCEPTION_DEFINE(MemoryError);
PYLITE_EXCEPTION_DEFINE(NameError);
PYLITE_EXCEPTION_DEFINE(OSError);
PYLITE_EXCEPTION_DEFINE(ReferenceError);
PYLITE_EXCEPTION_DEFINE(RuntimeError);
PYLITE_EXCEPTION_DEFINE(SyntaxError);
PYLITE_EXCEPTION_DEFINE(SystemError);
PYLITE_EXCEPTION_DEFINE(TypeError);
PYLITE_EXCEPTION_DEFINE(ValueError);
PYLITE_EXCEPTION_DEFINE(Warning);

// --> --> ArithmeticError
PYLITE_EXCEPTION_DEFINE(FloatingPointError);
PYLITE_EXCEPTION_DEFINE(OverflowError);
PYLITE_EXCEPTION_DEFINE(ZeroDivisionError);

// --> --> LookupError
PYLITE_EXCEPTION_DEFINE(IndexError);
PYLITE_EXCEPTION_DEFINE(KeyError);

// --> --> OSError
PYLITE_EXCEPTION_DEFINE(BlockingIOError);
PYLITE_EXCEPTION_DEFINE(ChildProcessError);
PYLITE_EXCEPTION_DEFINE(ConnectionError);
PYLITE_EXCEPTION_DEFINE(FileExistsError);
PYLITE_EXCEPTION_DEFINE(FileNotFoundError);
PYLITE_EXCEPTION_DEFINE(InterruptedError);
PYLITE_EXCEPTION_DEFINE(IsADirectoryError);
PYLITE_EXCEPTION_DEFINE(NotADirectoryError);
PYLITE_EXCEPTION_DEFINE(PermissionError);
PYLITE_EXCEPTION_DEFINE(ProcessLookupError);
PYLITE_EXCEPTION_DEFINE(TimeoutError);

// --> --> RuntimeError
PYLITE_EXCEPTION_DEFINE(NotImplementedError);
PYLITE_EXCEPTION_DEFINE(RecursionError);

// --> --> SyntaxError
PYLITE_EXCEPTION_DEFINE(IndentationError);

// --> --> Warning
PYLITE_EXCEPTION_DEFINE(DeprecationWarning);
PYLITE_EXCEPTION_DEFINE(PendingDeprecationWarning);
PYLITE_EXCEPTION_DEFINE(RuntimeWarning);
PYLITE_EXCEPTION_DEFINE(SyntaxWarning);
PYLITE_EXCEPTION_DEFINE(UserWarning);
PYLITE_EXCEPTION_DEFINE(FutureWarning);
PYLITE_EXCEPTION_DEFINE(ImportWarning);
PYLITE_EXCEPTION_DEFINE(UnicodeWarning);
PYLITE_EXCEPTION_DEFINE(BytesWarning);
PYLITE_EXCEPTION_DEFINE(ResourceWarning);

PyLiteBaseExceptionObject* pylt_obj_exception_new(PyLiteInterpreter *I, PyLiteTupleObject *args);
void pylt_obj_exception_free(PyLiteInterpreter *I, PyLiteBaseExceptionObject* self);

#endif
