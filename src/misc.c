
#include "misc.h"

void pylt_misc_static_objs_init(PyLiteInterpreter *I) {
    pl_static.str.__init__ = pylt_obj_str_new_from_cstr_static(I, "__init__", true);
    pl_static.str.__new__ = pylt_obj_str_new_from_cstr_static(I, "__new__", true);
    pl_static.str.__call__ = pylt_obj_str_new_from_cstr_static(I, "__call__", true);
    pl_static.str.__base__ = pylt_obj_str_new_from_cstr_static(I, "__base__", true);
    pl_static.str.__import__ = pylt_obj_str_new_from_cstr_static(I, "__import__", true);

    pl_static.str.__add__ = pylt_obj_str_new_from_cstr_static(I, "__add__", true);
    pl_static.str.__sub__ = pylt_obj_str_new_from_cstr_static(I, "__sub__", true);
    pl_static.str.__mul__ = pylt_obj_str_new_from_cstr_static(I, "__mul__", true);
    pl_static.str.__div__ = pylt_obj_str_new_from_cstr_static(I, "__div__", true);
    pl_static.str.__floordiv__ = pylt_obj_str_new_from_cstr_static(I, "__floordiv__", true);
    pl_static.str.__pos__ = pylt_obj_str_new_from_cstr_static(I, "__pos__", true);
    pl_static.str.__neg__ = pylt_obj_str_new_from_cstr_static(I, "__neg__", true);
    pl_static.str.__pow__ = pylt_obj_str_new_from_cstr_static(I, "__pow__", true);

    pl_static.str.__lshift__ = pylt_obj_str_new_from_cstr_static(I, "__lshift__", true);
    pl_static.str.__rshift__ = pylt_obj_str_new_from_cstr_static(I, "__rshift__", true);

    pl_static.str.__hash__ = pylt_obj_str_new_from_cstr_static(I, "__hash__", true);
    pl_static.str.__iter__ = pylt_obj_str_new_from_cstr_static(I, "__iter__", true);
    pl_static.str.__cmp__ = pylt_obj_str_new_from_cstr_static(I, "__cmp__", true);
    pl_static.str.__eq__ = pylt_obj_str_new_from_cstr_static(I, "__eq__", true);
    pl_static.str.__setattr__ = pylt_obj_str_new_from_cstr_static(I, "__setattr__", true);
    pl_static.str.__getattr__ = pylt_obj_str_new_from_cstr_static(I, "__getattr__", true);
    pl_static.str.__setitem__ = pylt_obj_str_new_from_cstr_static(I, "__setitem__", true);
	pl_static.str.__getitem__ = pylt_obj_str_new_from_cstr_static(I, "__getitem__", true);
	pl_static.str.__defaults__ = pylt_obj_str_new_from_cstr_static(I, "__defaults__", true);
	pl_static.str.__parameters__ = pylt_obj_str_new_from_cstr_static(I, "__parameters__", true);

    pl_static.str.id = pylt_obj_str_new_from_cstr_static(I, "id", true);
    pl_static.str.dir = pylt_obj_str_new_from_cstr_static(I, "dir", true);
    pl_static.str.len = pylt_obj_str_new_from_cstr_static(I, "len", true);
    pl_static.str.hash = pylt_obj_str_new_from_cstr_static(I, "hash", true);
    pl_static.str.iter = pylt_obj_str_new_from_cstr_static(I, "iter", true);
    pl_static.str.repr = pylt_obj_str_new_from_cstr_static(I, "repr", true);
    pl_static.str.super = pylt_obj_str_new_from_cstr_static(I, "super", true);
    pl_static.str.isinstance = pylt_obj_str_new_from_cstr_static(I, "isinstance", true);
    pl_static.str.print = pylt_obj_str_new_from_cstr_static(I, "print", true);
    pl_static.str.pow = pylt_obj_str_new_from_cstr_static(I, "pow", true);
    pl_static.str.setattr = pylt_obj_str_new_from_cstr_static(I, "setattr", true);
    pl_static.str.getattr = pylt_obj_str_new_from_cstr_static(I, "getattr", true);
    pl_static.str.range = pylt_obj_str_new_from_cstr_static(I, "range", true);

    pl_static.str.object = pylt_obj_str_new_from_cstr_static(I, "object", true);
    pl_static.str.int_ = pylt_obj_str_new_from_cstr_static(I, "int", true);
    pl_static.str.float_ = pylt_obj_str_new_from_cstr_static(I, "float", true);
    pl_static.str.bool_ = pylt_obj_str_new_from_cstr_static(I, "bool_", true);
    pl_static.str.str = pylt_obj_str_new_from_cstr_static(I, "str", true);
    pl_static.str.bytes = pylt_obj_str_new_from_cstr_static(I, "bytes", true);
    pl_static.str.set = pylt_obj_str_new_from_cstr_static(I, "set", true);
    pl_static.str.list = pylt_obj_str_new_from_cstr_static(I, "list", true);
	pl_static.str.tuple = pylt_obj_str_new_from_cstr_static(I, "tuple", true);
	pl_static.str.dict = pylt_obj_str_new_from_cstr_static(I, "dict", true);
	pl_static.str.unusual = pylt_obj_str_new_from_cstr_static(I, "unusual", true);

    pl_static.str.module = pylt_obj_str_new_from_cstr_static(I, "module", true);
    pl_static.str.function = pylt_obj_str_new_from_cstr_static(I, "function", true);
    pl_static.str.cfunction = pylt_obj_str_new_from_cstr_static(I, "cfunction", true);
    pl_static.str.code = pylt_obj_str_new_from_cstr_static(I, "code", true);
    pl_static.str.type = pylt_obj_str_new_from_cstr_static(I, "type", true);
    pl_static.str.iterator = pylt_obj_str_new_from_cstr_static(I, "iterator", true);
    pl_static.str.property_ = pylt_obj_str_new_from_cstr_static(I, "property", true);
    pl_static.str.NoneType = pylt_obj_str_new_from_cstr_static(I, "NoneType", true);

    pl_static.str.mro = pylt_obj_str_new_from_cstr_static(I, "mro", true);

    pl_static.str.join = pylt_obj_str_new_from_cstr_static(I, "join", true);
    pl_static.str.sub = pylt_obj_str_new_from_cstr_static(I, "sub", true);
    pl_static.str.start = pylt_obj_str_new_from_cstr_static(I, "start", true);
    pl_static.str.end = pylt_obj_str_new_from_cstr_static(I, "end", true);

    pl_static.str.is_integer = pylt_obj_str_new_from_cstr_static(I, "is_integer", true);

    pl_static.str.add = pylt_obj_str_new_from_cstr_static(I, "add", true);
    pl_static.str.clear = pylt_obj_str_new_from_cstr_static(I, "clear", true);
    pl_static.str.copy = pylt_obj_str_new_from_cstr_static(I, "copy", true);
    pl_static.str.pop = pylt_obj_str_new_from_cstr_static(I, "pop", true);
    pl_static.str.remove = pylt_obj_str_new_from_cstr_static(I, "remove", true);

    pl_static.str.append = pylt_obj_str_new_from_cstr_static(I, "append", true);
    pl_static.str.count = pylt_obj_str_new_from_cstr_static(I, "count", true);
    pl_static.str.index = pylt_obj_str_new_from_cstr_static(I, "index", true);
    pl_static.str.extend = pylt_obj_str_new_from_cstr_static(I, "extend", true);
    pl_static.str.insert = pylt_obj_str_new_from_cstr_static(I, "insert", true);
    pl_static.str.reverse = pylt_obj_str_new_from_cstr_static(I, "reverse", true);

	pl_static.str.unknown = pylt_obj_str_new_from_cstr_static(I, "unknown", true);
	pl_static.str.param_args = pylt_obj_str_new_from_cstr_static(I, "param_args", true);
	pl_static.str.param_kwargs = pylt_obj_str_new_from_cstr_static(I, "param_kwargs", true);
	pl_static.str.param_undefined = pylt_obj_str_new_from_cstr_static(I, "param_undefined", true);

    pl_static.str.fget = pylt_obj_str_new_from_cstr_static(I, "fget", true);
    pl_static.str.fset = pylt_obj_str_new_from_cstr_static(I, "fset", true);

    pl_static.str.None = pylt_obj_str_new_from_cstr_static(I, "None", true);
    pl_static.str.True = pylt_obj_str_new_from_cstr_static(I, "True", true);
    pl_static.str.False = pylt_obj_str_new_from_cstr_static(I, "False", true);

    pl_static.str.math = pylt_obj_str_new_from_cstr_static(I, "math", true);
    pl_static.str.builtins = pylt_obj_str_new_from_cstr_static(I, "builtins", true);

    pl_static.str.cls = pylt_obj_str_new_from_cstr_static(I, "cls", true);
    pl_static.str.self = pylt_obj_str_new_from_cstr_static(I, "self", true);
    pl_static.str.args = pylt_obj_str_new_from_cstr_static(I, "args", true);
    pl_static.str.kwargs = pylt_obj_str_new_from_cstr_static(I, "kwargs", true);
    pl_static.str.param1 = pylt_obj_str_new_from_cstr_static(I, "param1", true);
    pl_static.str.param2 = pylt_obj_str_new_from_cstr_static(I, "param2", true);
    pl_static.str.param3 = pylt_obj_str_new_from_cstr_static(I, "param3", true);
    pl_static.str.param4 = pylt_obj_str_new_from_cstr_static(I, "param4", true);

    pl_static.str.x = pylt_obj_str_new_from_cstr_static(I, "x", true);
    pl_static.str.y = pylt_obj_str_new_from_cstr_static(I, "y", true);

    pl_static.str.BaseException = pylt_obj_str_new_from_cstr_static(I, "BaseException", true);
    pl_static.str.SystemExit = pylt_obj_str_new_from_cstr_static(I, "SystemExit", true);
    pl_static.str.KeyboardInterrupt = pylt_obj_str_new_from_cstr_static(I, "KeyboardInterrupt", true);
    pl_static.str.GeneratorExit = pylt_obj_str_new_from_cstr_static(I, "GeneratorExit", true);
    pl_static.str.Exception = pylt_obj_str_new_from_cstr_static(I, "Exception", true);
    pl_static.str.StopIteration = pylt_obj_str_new_from_cstr_static(I, "StopIteration", true);
    pl_static.str.StopAsyncIteration = pylt_obj_str_new_from_cstr_static(I, "StopAsyncIteration", true);
    pl_static.str.ArithmeticError = pylt_obj_str_new_from_cstr_static(I, "ArithmeticError", true);
    pl_static.str.FloatingPointError = pylt_obj_str_new_from_cstr_static(I, "FloatingPointError", true);
    pl_static.str.OverflowError = pylt_obj_str_new_from_cstr_static(I, "OverflowError", true);
    pl_static.str.ZeroDivisionError = pylt_obj_str_new_from_cstr_static(I, "ZeroDivisionError", true);
    pl_static.str.AssertionError = pylt_obj_str_new_from_cstr_static(I, "AssertionError", true);
    pl_static.str.AttributeError = pylt_obj_str_new_from_cstr_static(I, "AttributeError", true);
    pl_static.str.BufferError = pylt_obj_str_new_from_cstr_static(I, "BufferError", true);
    pl_static.str.EOFError = pylt_obj_str_new_from_cstr_static(I, "EOFError", true);
    pl_static.str.ImportError = pylt_obj_str_new_from_cstr_static(I, "ImportError", true);
    pl_static.str.LookupError = pylt_obj_str_new_from_cstr_static(I, "LookupError", true);
    pl_static.str.IndexError = pylt_obj_str_new_from_cstr_static(I, "IndexError", true);
    pl_static.str.KeyError = pylt_obj_str_new_from_cstr_static(I, "KeyError", true);
    pl_static.str.MemoryError = pylt_obj_str_new_from_cstr_static(I, "MemoryError", true);
    pl_static.str.NameError = pylt_obj_str_new_from_cstr_static(I, "NameError", true);
    pl_static.str.UnboundLocalError = pylt_obj_str_new_from_cstr_static(I, "UnboundLocalError", true);
    pl_static.str.OSError = pylt_obj_str_new_from_cstr_static(I, "OSError", true);
    pl_static.str.BlockingIOError = pylt_obj_str_new_from_cstr_static(I, "BlockingIOError", true);
    pl_static.str.ChildProcessError = pylt_obj_str_new_from_cstr_static(I, "ChildProcessError", true);
    pl_static.str.ConnectionError = pylt_obj_str_new_from_cstr_static(I, "ConnectionError", true);
    pl_static.str.BrokenPipeError = pylt_obj_str_new_from_cstr_static(I, "BrokenPipeError", true);
    pl_static.str.ConnectionAbortedError = pylt_obj_str_new_from_cstr_static(I, "ConnectionAbortedError", true);
    pl_static.str.ConnectionRefusedError = pylt_obj_str_new_from_cstr_static(I, "ConnectionRefusedError", true);
    pl_static.str.ConnectionResetError = pylt_obj_str_new_from_cstr_static(I, "ConnectionResetError", true);
    pl_static.str.FileExistsError = pylt_obj_str_new_from_cstr_static(I, "FileExistsError", true);
    pl_static.str.FileNotFoundError = pylt_obj_str_new_from_cstr_static(I, "FileNotFoundError", true);
    pl_static.str.InterruptedError = pylt_obj_str_new_from_cstr_static(I, "InterruptedError", true);
    pl_static.str.IsADirectoryError = pylt_obj_str_new_from_cstr_static(I, "IsADirectoryError", true);
    pl_static.str.NotADirectoryError = pylt_obj_str_new_from_cstr_static(I, "NotADirectoryError", true);
    pl_static.str.PermissionError = pylt_obj_str_new_from_cstr_static(I, "PermissionError", true);
    pl_static.str.ProcessLookupError = pylt_obj_str_new_from_cstr_static(I, "ProcessLookupError", true);
    pl_static.str.TimeoutError = pylt_obj_str_new_from_cstr_static(I, "TimeoutError", true);
    pl_static.str.ReferenceError = pylt_obj_str_new_from_cstr_static(I, "ReferenceError", true);
    pl_static.str.RuntimeError = pylt_obj_str_new_from_cstr_static(I, "RuntimeError", true);
    pl_static.str.NotImplementedError = pylt_obj_str_new_from_cstr_static(I, "NotImplementedError", true);
    pl_static.str.RecursionError = pylt_obj_str_new_from_cstr_static(I, "RecursionError", true);
    pl_static.str.SyntaxError = pylt_obj_str_new_from_cstr_static(I, "SyntaxError", true);
    pl_static.str.IndentationError = pylt_obj_str_new_from_cstr_static(I, "IndentationError", true);
    pl_static.str.TabError = pylt_obj_str_new_from_cstr_static(I, "TabError", true);
    pl_static.str.SystemError = pylt_obj_str_new_from_cstr_static(I, "SystemError", true);
    pl_static.str.TypeError = pylt_obj_str_new_from_cstr_static(I, "TypeError", true);
    pl_static.str.ValueError = pylt_obj_str_new_from_cstr_static(I, "ValueError", true);
    pl_static.str.UnicodeError = pylt_obj_str_new_from_cstr_static(I, "UnicodeError", true);
    pl_static.str.UnicodeDecodeError = pylt_obj_str_new_from_cstr_static(I, "UnicodeDecodeError", true);
    pl_static.str.UnicodeEncodeError = pylt_obj_str_new_from_cstr_static(I, "UnicodeEncodeError", true);
    pl_static.str.UnicodeTranslateError = pylt_obj_str_new_from_cstr_static(I, "UnicodeTranslateError", true);
    pl_static.str.Warning = pylt_obj_str_new_from_cstr_static(I, "Warning", true);
    pl_static.str.DeprecationWarning = pylt_obj_str_new_from_cstr_static(I, "DeprecationWarning", true);
    pl_static.str.PendingDeprecationWarning = pylt_obj_str_new_from_cstr_static(I, "PendingDeprecationWarning", true);
    pl_static.str.RuntimeWarning = pylt_obj_str_new_from_cstr_static(I, "RuntimeWarning", true);
    pl_static.str.SyntaxWarning = pylt_obj_str_new_from_cstr_static(I, "SyntaxWarning", true);
    pl_static.str.UserWarning = pylt_obj_str_new_from_cstr_static(I, "UserWarning", true);
    pl_static.str.FutureWarning = pylt_obj_str_new_from_cstr_static(I, "FutureWarning", true);
    pl_static.str.ImportWarning = pylt_obj_str_new_from_cstr_static(I, "ImportWarning", true);
    pl_static.str.UnicodeWarning = pylt_obj_str_new_from_cstr_static(I, "UnicodeWarning", true);
    pl_static.str.BytesWarning = pylt_obj_str_new_from_cstr_static(I, "BytesWarning", true);
    pl_static.str.ResourceWarning = pylt_obj_str_new_from_cstr_static(I, "ResourceWarning", true);

    pl_static.str.TMPL_OBJECT_TO_STR = pylt_obj_str_new_from_cstr_static(I, "<%s object at %p>", true);
    pl_static.str.TMPL_CLASS_TO_STR = pylt_obj_str_new_from_cstr_static(I, "<class '%s'>", true);
    pl_static.str.TMPL_MODULE_TO_STR = pylt_obj_str_new_from_cstr_static(I, "<module '%s'>", true);
    pl_static.str.TMPL_FUNCTION_TO_STR = pylt_obj_str_new_from_cstr_static(I, "<function %s at %p>", true);
    pl_static.str.TMPL_CFUNCTION_TO_STR = pylt_obj_str_new_from_cstr_static(I, "<cfunction %s at %p>", true);

    pl_static.str.TMPL_EMPTY_STR = pylt_obj_str_new_from_cstr_static(I, "", true);
    pl_static.str.TMPL_EMPTY_SET = pylt_obj_str_new_from_cstr_static(I, "set()", true);
    pl_static.str.TMPL_EMPTY_LIST = pylt_obj_str_new_from_cstr_static(I, "[]", true);
    pl_static.str.TMPL_EMPTY_TUPLE = pylt_obj_str_new_from_cstr_static(I, "()", true);
    pl_static.str.TMPL_EMPTY_DICT = pylt_obj_str_new_from_cstr_static(I, "{}", true);
}
