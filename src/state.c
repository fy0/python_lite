
#include "state.h"

void pylt_state_err(PyLiteState *state) {
    exit(-1);
}

PyLiteState* pylt_state_new() {
    PyLiteState *state = pylt_realloc(NULL, sizeof(PyLiteState));
    pylt_state_init(state);
    return state;
}

void pylt_state_free(PyLiteState *state) {
    pylt_state_finalize(state);
    pylt_free(state);
}

void pylt_state_init(PyLiteState *state) {
    kv_init(state->cls_base);
    state->modules = pylt_obj_dict_new(state);
    state->cache_str = pylt_obj_set_new(state);
    state->cache_bytes = pylt_obj_set_new(state);
    state->error_code = 0;

    state->lexer = pylt_realloc(NULL, sizeof(LexState));
    state->parser = pylt_realloc(NULL, sizeof(ParserState));

    pylt_gc_init(state);
    pylt_lex_init(state, state->lexer, NULL);
    pylt_parser_init(state, state->parser, state->lexer);
    pylt_utils_static_objs_init(state);
    pylt_vm_init(state, &state->vm);
}

void pylt_state_finalize(PyLiteState *state) {
    pylt_lex_finalize(state, state->lexer);
    pylt_parser_finalize(state, state->parser);
    pylt_vm_finalize(state);

    // free types
    for (pl_uint_t i = 0; i < kv_size(state->cls_base); ++i) {
        pylt_obj_type_free(state, kv_A(state->cls_base, i));
    }
    kv_destroy(state->cls_base);

    // free modules
    PyLiteDictObject *dict = state->modules;
    for (pl_int_t it = pylt_obj_dict_begin(state, dict); it != pylt_obj_dict_end(state, dict); pylt_obj_dict_next(state, dict, &it)) {
        PyLiteObject *k, *v;
        pylt_obj_dict_keyvalue(state, dict, it, &k, &v);
        pylt_obj_safefree(state, k);
        pylt_obj_safefree(state, v);
    }
 
    pylt_obj_dict_free(state, state->modules);
    pylt_obj_set_free(state, state->cache_str);
    pylt_obj_set_free(state, state->cache_bytes);
    pylt_gc_finalize(state);

    pylt_free(state->lexer);
    pylt_free(state->parser);
}

void pylt_state_load_stream(PyLiteState *state, StringStream *ss) {
    state->lexer->ss = ss;
    pylt_parser_parse(state->parser);
}

void pylt_state_run(PyLiteState *state) {
    pylt_vm_run(state, state->parser->info->code);
}

void pylt_utils_static_objs_init(PyLiteState *state) {
    pl_static.str.__init__ = pylt_obj_str_new_from_c_str(state, "__init__", true);
    pl_static.str.__new__ = pylt_obj_str_new_from_c_str(state, "__new__", true);
    pl_static.str.__call__ = pylt_obj_str_new_from_c_str(state, "__call__", true);
    pl_static.str.__base__ = pylt_obj_str_new_from_c_str(state, "__base__", true);
    pl_static.str.__import__ = pylt_obj_str_new_from_c_str(state, "__import__", true);

    pl_static.str.__add__ = pylt_obj_str_new_from_c_str(state, "__add__", true);
    pl_static.str.__sub__ = pylt_obj_str_new_from_c_str(state, "__sub__", true);
    pl_static.str.__mul__ = pylt_obj_str_new_from_c_str(state, "__mul__", true);
    pl_static.str.__div__ = pylt_obj_str_new_from_c_str(state, "__div__", true);
    pl_static.str.__floordiv__ = pylt_obj_str_new_from_c_str(state, "__floordiv__", true);
    pl_static.str.__pos__ = pylt_obj_str_new_from_c_str(state, "__pos__", true);
    pl_static.str.__neg__ = pylt_obj_str_new_from_c_str(state, "__neg__", true);
    pl_static.str.__pow__ = pylt_obj_str_new_from_c_str(state, "__pow__", true);

    pl_static.str.__lshift__ = pylt_obj_str_new_from_c_str(state, "__lshift__", true);
    pl_static.str.__rshift__ = pylt_obj_str_new_from_c_str(state, "__rshift__", true);

    pl_static.str.__hash__ = pylt_obj_str_new_from_c_str(state, "__hash__", true);
    pl_static.str.__iter__ = pylt_obj_str_new_from_c_str(state, "__iter__", true);
    pl_static.str.__cmp__ = pylt_obj_str_new_from_c_str(state, "__cmp__", true);
    pl_static.str.__eq__ = pylt_obj_str_new_from_c_str(state, "__eq__", true);
    pl_static.str.__setattr__ = pylt_obj_str_new_from_c_str(state, "__setattr__", true);
    pl_static.str.__getattr__ = pylt_obj_str_new_from_c_str(state, "__getattr__", true);
    pl_static.str.__setitem__ = pylt_obj_str_new_from_c_str(state, "__setitem__", true);
    pl_static.str.__getitem__ = pylt_obj_str_new_from_c_str(state, "__getitem__", true);

    pl_static.str.id = pylt_obj_str_new_from_c_str(state, "id", true);
    pl_static.str.dir = pylt_obj_str_new_from_c_str(state, "dir", true);
    pl_static.str.len = pylt_obj_str_new_from_c_str(state, "len", true);
    pl_static.str.hash = pylt_obj_str_new_from_c_str(state, "hash", true);
    pl_static.str.iter = pylt_obj_str_new_from_c_str(state, "iter", true);
    pl_static.str.super = pylt_obj_str_new_from_c_str(state, "super", true);
    pl_static.str.isinstance = pylt_obj_str_new_from_c_str(state, "isinstance", true);
    pl_static.str.print = pylt_obj_str_new_from_c_str(state, "print", true);
    pl_static.str.pow = pylt_obj_str_new_from_c_str(state, "pow", true);
    pl_static.str.setattr = pylt_obj_str_new_from_c_str(state, "setattr", true);
    pl_static.str.getattr = pylt_obj_str_new_from_c_str(state, "getattr", true);
    pl_static.str.range = pylt_obj_str_new_from_c_str(state, "range", true);

    pl_static.str.object = pylt_obj_str_new_from_c_str(state, "object", true);
    pl_static.str.int_ = pylt_obj_str_new_from_c_str(state, "int", true);
    pl_static.str.float_ = pylt_obj_str_new_from_c_str(state, "float", true);
    pl_static.str.bool_ = pylt_obj_str_new_from_c_str(state, "bool_", true);
    pl_static.str.str = pylt_obj_str_new_from_c_str(state, "str", true);
    pl_static.str.bytes = pylt_obj_str_new_from_c_str(state, "bytes", true);
    pl_static.str.set = pylt_obj_str_new_from_c_str(state, "set", true);
    pl_static.str.list = pylt_obj_str_new_from_c_str(state, "list", true);
    pl_static.str.tuple = pylt_obj_str_new_from_c_str(state, "tuple", true);
    pl_static.str.dict = pylt_obj_str_new_from_c_str(state, "dict", true);

    pl_static.str.module = pylt_obj_str_new_from_c_str(state, "module", true);
    pl_static.str.function = pylt_obj_str_new_from_c_str(state, "function", true);
    pl_static.str.cfunction = pylt_obj_str_new_from_c_str(state, "cfunction", true);
    pl_static.str.code = pylt_obj_str_new_from_c_str(state, "code", true);
    pl_static.str.type = pylt_obj_str_new_from_c_str(state, "type", true);
    pl_static.str.iterator = pylt_obj_str_new_from_c_str(state, "iterator", true);
    pl_static.str.property_ = pylt_obj_str_new_from_c_str(state, "property", true);
    pl_static.str.none = pylt_obj_str_new_from_c_str(state, "none", true);

    pl_static.str.mro = pylt_obj_str_new_from_c_str(state, "mro", true);

    pl_static.str.sub = pylt_obj_str_new_from_c_str(state, "sub", true);
    pl_static.str.start = pylt_obj_str_new_from_c_str(state, "start", true);
    pl_static.str.end = pylt_obj_str_new_from_c_str(state, "end", true);

    pl_static.str.is_integer = pylt_obj_str_new_from_c_str(state, "is_integer", true);

    pl_static.str.add = pylt_obj_str_new_from_c_str(state, "add", true);
    pl_static.str.clear = pylt_obj_str_new_from_c_str(state, "clear", true);
    pl_static.str.copy = pylt_obj_str_new_from_c_str(state, "copy", true);
    pl_static.str.pop = pylt_obj_str_new_from_c_str(state, "pop", true);
    pl_static.str.remove = pylt_obj_str_new_from_c_str(state, "remove", true);

    pl_static.str.append = pylt_obj_str_new_from_c_str(state, "append", true);
    pl_static.str.count = pylt_obj_str_new_from_c_str(state, "count", true);
    pl_static.str.index = pylt_obj_str_new_from_c_str(state, "index", true);
    pl_static.str.extend = pylt_obj_str_new_from_c_str(state, "extend", true);
    pl_static.str.insert = pylt_obj_str_new_from_c_str(state, "insert", true);
    pl_static.str.reverse = pylt_obj_str_new_from_c_str(state, "reverse", true);

	pl_static.str.None = pylt_obj_str_new_from_c_str(state, "None", true);
	pl_static.str.True = pylt_obj_str_new_from_c_str(state, "True", true);
	pl_static.str.False = pylt_obj_str_new_from_c_str(state, "False", true);

    pl_static.str.math = pylt_obj_str_new_from_c_str(state, "math", true);
    pl_static.str.builtins = pylt_obj_str_new_from_c_str(state, "builtins", true);

    pl_static.str.cls = pylt_obj_str_new_from_c_str(state, "cls", true);
    pl_static.str.self = pylt_obj_str_new_from_c_str(state, "self", true);
    pl_static.str.args = pylt_obj_str_new_from_c_str(state, "args", true);
    pl_static.str.kwargs = pylt_obj_str_new_from_c_str(state, "kwargs", true);
    pl_static.str.param1 = pylt_obj_str_new_from_c_str(state, "param1", true);
    pl_static.str.param2 = pylt_obj_str_new_from_c_str(state, "param2", true);
    pl_static.str.param3 = pylt_obj_str_new_from_c_str(state, "param3", true);
    pl_static.str.param4 = pylt_obj_str_new_from_c_str(state, "param4", true);
     
    pl_static.str.x = pylt_obj_str_new_from_c_str(state, "x", true);
    pl_static.str.y = pylt_obj_str_new_from_c_str(state, "y", true);

    pl_static.str.BaseException = pylt_obj_str_new_from_c_str(state, "BaseException", true);
    pl_static.str.SystemExit = pylt_obj_str_new_from_c_str(state, "SystemExit", true);
    pl_static.str.KeyboardInterrupt = pylt_obj_str_new_from_c_str(state, "KeyboardInterrupt", true);
    pl_static.str.GeneratorExit = pylt_obj_str_new_from_c_str(state, "GeneratorExit", true);
    pl_static.str.Exception = pylt_obj_str_new_from_c_str(state, "Exception", true);
    pl_static.str.StopIteration = pylt_obj_str_new_from_c_str(state, "StopIteration", true);
    pl_static.str.StopAsyncIteration = pylt_obj_str_new_from_c_str(state, "StopAsyncIteration", true);
    pl_static.str.ArithmeticError = pylt_obj_str_new_from_c_str(state, "ArithmeticError", true);
    pl_static.str.FloatingPointError = pylt_obj_str_new_from_c_str(state, "FloatingPointError", true);
    pl_static.str.OverflowError = pylt_obj_str_new_from_c_str(state, "OverflowError", true);
    pl_static.str.ZeroDivisionError = pylt_obj_str_new_from_c_str(state, "ZeroDivisionError", true);
    pl_static.str.AssertionError = pylt_obj_str_new_from_c_str(state, "AssertionError", true);
    pl_static.str.AttributeError = pylt_obj_str_new_from_c_str(state, "AttributeError", true);
    pl_static.str.BufferError = pylt_obj_str_new_from_c_str(state, "BufferError", true);
    pl_static.str.EOFError = pylt_obj_str_new_from_c_str(state, "EOFError", true);
    pl_static.str.ImportError = pylt_obj_str_new_from_c_str(state, "ImportError", true);
    pl_static.str.LookupError = pylt_obj_str_new_from_c_str(state, "LookupError", true);
    pl_static.str.IndexError = pylt_obj_str_new_from_c_str(state, "IndexError", true);
    pl_static.str.KeyError = pylt_obj_str_new_from_c_str(state, "KeyError", true);
    pl_static.str.MemoryError = pylt_obj_str_new_from_c_str(state, "MemoryError", true);
    pl_static.str.NameError = pylt_obj_str_new_from_c_str(state, "NameError", true);
    pl_static.str.UnboundLocalError = pylt_obj_str_new_from_c_str(state, "UnboundLocalError", true);
    pl_static.str.OSError = pylt_obj_str_new_from_c_str(state, "OSError", true);
    pl_static.str.BlockingIOError = pylt_obj_str_new_from_c_str(state, "BlockingIOError", true);
    pl_static.str.ChildProcessError = pylt_obj_str_new_from_c_str(state, "ChildProcessError", true);
    pl_static.str.ConnectionError = pylt_obj_str_new_from_c_str(state, "ConnectionError", true);
    pl_static.str.BrokenPipeError = pylt_obj_str_new_from_c_str(state, "BrokenPipeError", true);
    pl_static.str.ConnectionAbortedError = pylt_obj_str_new_from_c_str(state, "ConnectionAbortedError", true);
    pl_static.str.ConnectionRefusedError = pylt_obj_str_new_from_c_str(state, "ConnectionRefusedError", true);
    pl_static.str.ConnectionResetError = pylt_obj_str_new_from_c_str(state, "ConnectionResetError", true);
    pl_static.str.FileExistsError = pylt_obj_str_new_from_c_str(state, "FileExistsError", true);
    pl_static.str.FileNotFoundError = pylt_obj_str_new_from_c_str(state, "FileNotFoundError", true);
    pl_static.str.InterruptedError = pylt_obj_str_new_from_c_str(state, "InterruptedError", true);
    pl_static.str.IsADirectoryError = pylt_obj_str_new_from_c_str(state, "IsADirectoryError", true);
    pl_static.str.NotADirectoryError = pylt_obj_str_new_from_c_str(state, "NotADirectoryError", true);
    pl_static.str.PermissionError = pylt_obj_str_new_from_c_str(state, "PermissionError", true);
    pl_static.str.ProcessLookupError = pylt_obj_str_new_from_c_str(state, "ProcessLookupError", true);
    pl_static.str.TimeoutError = pylt_obj_str_new_from_c_str(state, "TimeoutError", true);
    pl_static.str.ReferenceError = pylt_obj_str_new_from_c_str(state, "ReferenceError", true);
    pl_static.str.RuntimeError = pylt_obj_str_new_from_c_str(state, "RuntimeError", true);
    pl_static.str.NotImplementedError = pylt_obj_str_new_from_c_str(state, "NotImplementedError", true);
    pl_static.str.RecursionError = pylt_obj_str_new_from_c_str(state, "RecursionError", true);
    pl_static.str.SyntaxError = pylt_obj_str_new_from_c_str(state, "SyntaxError", true);
    pl_static.str.IndentationError = pylt_obj_str_new_from_c_str(state, "IndentationError", true);
    pl_static.str.TabError = pylt_obj_str_new_from_c_str(state, "TabError", true);
    pl_static.str.SystemError = pylt_obj_str_new_from_c_str(state, "SystemError", true);
    pl_static.str.TypeError = pylt_obj_str_new_from_c_str(state, "TypeError", true);
    pl_static.str.ValueError = pylt_obj_str_new_from_c_str(state, "ValueError", true);
    pl_static.str.UnicodeError = pylt_obj_str_new_from_c_str(state, "UnicodeError", true);
    pl_static.str.UnicodeDecodeError = pylt_obj_str_new_from_c_str(state, "UnicodeDecodeError", true);
    pl_static.str.UnicodeEncodeError = pylt_obj_str_new_from_c_str(state, "UnicodeEncodeError", true);
    pl_static.str.UnicodeTranslateError = pylt_obj_str_new_from_c_str(state, "UnicodeTranslateError", true);
    pl_static.str.Warning = pylt_obj_str_new_from_c_str(state, "Warning", true);
    pl_static.str.DeprecationWarning = pylt_obj_str_new_from_c_str(state, "DeprecationWarning", true);
    pl_static.str.PendingDeprecationWarning = pylt_obj_str_new_from_c_str(state, "PendingDeprecationWarning", true);
    pl_static.str.RuntimeWarning = pylt_obj_str_new_from_c_str(state, "RuntimeWarning", true);
    pl_static.str.SyntaxWarning = pylt_obj_str_new_from_c_str(state, "SyntaxWarning", true);
    pl_static.str.UserWarning = pylt_obj_str_new_from_c_str(state, "UserWarning", true);
    pl_static.str.FutureWarning = pylt_obj_str_new_from_c_str(state, "FutureWarning", true);
    pl_static.str.ImportWarning = pylt_obj_str_new_from_c_str(state, "ImportWarning", true);
    pl_static.str.UnicodeWarning = pylt_obj_str_new_from_c_str(state, "UnicodeWarning", true);
    pl_static.str.BytesWarning = pylt_obj_str_new_from_c_str(state, "BytesWarning", true);
    pl_static.str.ResourceWarning = pylt_obj_str_new_from_c_str(state, "ResourceWarning", true);

	pl_static.str.TMPL_OBJECT_TO_STR = pylt_obj_str_new_from_c_str(state, "<%s object at %p>", true);
	pl_static.str.TMPL_CLASS_TO_STR = pylt_obj_str_new_from_c_str(state, "<class '%s'>", true);
	pl_static.str.TMPL_MODULE_TO_STR = pylt_obj_str_new_from_c_str(state, "<module '%s'>", true);
	pl_static.str.TMPL_FUNCTION_TO_STR = pylt_obj_str_new_from_c_str(state, "<function %s at %p>", true);
	pl_static.str.TMPL_CFUNCTION_TO_STR = pylt_obj_str_new_from_c_str(state, "<cfunction %s at %p>", true);

	pl_static.str.TMPL_EMPTY_SET = pylt_obj_str_new_from_c_str(state, "set()", true);
	pl_static.str.TMPL_EMPTY_LIST = pylt_obj_str_new_from_c_str(state, "[]", true);
	pl_static.str.TMPL_EMPTY_TUPLE = pylt_obj_str_new_from_c_str(state, "()", true);
	pl_static.str.TMPL_EMPTY_DICT = pylt_obj_str_new_from_c_str(state, "{}", true);

    pylt_gc_static_add(state, castobj(pl_static.str.__init__));
    pylt_gc_static_add(state, castobj(pl_static.str.__new__));
    pylt_gc_static_add(state, castobj(pl_static.str.__call__));
    pylt_gc_static_add(state, castobj(pl_static.str.__base__));
    pylt_gc_static_add(state, castobj(pl_static.str.__import__));

    pylt_gc_static_add(state, castobj(pl_static.str.__add__));
    pylt_gc_static_add(state, castobj(pl_static.str.__sub__));
    pylt_gc_static_add(state, castobj(pl_static.str.__mul__));
    pylt_gc_static_add(state, castobj(pl_static.str.__div__));
    pylt_gc_static_add(state, castobj(pl_static.str.__floordiv__));
    pylt_gc_static_add(state, castobj(pl_static.str.__pos__));
    pylt_gc_static_add(state, castobj(pl_static.str.__neg__));
    pylt_gc_static_add(state, castobj(pl_static.str.__pow__));

    pylt_gc_static_add(state, castobj(pl_static.str.__lshift__));
    pylt_gc_static_add(state, castobj(pl_static.str.__rshift__));

    pylt_gc_static_add(state, castobj(pl_static.str.__hash__));
    pylt_gc_static_add(state, castobj(pl_static.str.__iter__));
    pylt_gc_static_add(state, castobj(pl_static.str.__cmp__));
    pylt_gc_static_add(state, castobj(pl_static.str.__eq__));
    pylt_gc_static_add(state, castobj(pl_static.str.__setattr__));
    pylt_gc_static_add(state, castobj(pl_static.str.__getattr__));
    pylt_gc_static_add(state, castobj(pl_static.str.__setitem__));
    pylt_gc_static_add(state, castobj(pl_static.str.__getitem__));

    pylt_gc_static_add(state, castobj(pl_static.str.id));
    pylt_gc_static_add(state, castobj(pl_static.str.dir));
    pylt_gc_static_add(state, castobj(pl_static.str.len));
    pylt_gc_static_add(state, castobj(pl_static.str.hash));
    pylt_gc_static_add(state, castobj(pl_static.str.iter));
    pylt_gc_static_add(state, castobj(pl_static.str.super));
    pylt_gc_static_add(state, castobj(pl_static.str.isinstance));
    pylt_gc_static_add(state, castobj(pl_static.str.print));
    pylt_gc_static_add(state, castobj(pl_static.str.pow));
    pylt_gc_static_add(state, castobj(pl_static.str.setattr));
    pylt_gc_static_add(state, castobj(pl_static.str.getattr));
    pylt_gc_static_add(state, castobj(pl_static.str.range));

    pylt_gc_static_add(state, castobj(pl_static.str.object));
    pylt_gc_static_add(state, castobj(pl_static.str.int_));
    pylt_gc_static_add(state, castobj(pl_static.str.float_));
    pylt_gc_static_add(state, castobj(pl_static.str.bool_));
    pylt_gc_static_add(state, castobj(pl_static.str.str));
    pylt_gc_static_add(state, castobj(pl_static.str.bytes));
    pylt_gc_static_add(state, castobj(pl_static.str.set));
    pylt_gc_static_add(state, castobj(pl_static.str.list));
    pylt_gc_static_add(state, castobj(pl_static.str.tuple));
    pylt_gc_static_add(state, castobj(pl_static.str.dict));

    pylt_gc_static_add(state, castobj(pl_static.str.module));
    pylt_gc_static_add(state, castobj(pl_static.str.function));
    pylt_gc_static_add(state, castobj(pl_static.str.cfunction));
    pylt_gc_static_add(state, castobj(pl_static.str.code));
    pylt_gc_static_add(state, castobj(pl_static.str.type));
    pylt_gc_static_add(state, castobj(pl_static.str.iterator));
    pylt_gc_static_add(state, castobj(pl_static.str.property_));
    pylt_gc_static_add(state, castobj(pl_static.str.none));

    // object
    pylt_gc_static_add(state, castobj(pl_static.str.mro));

    // str
    pylt_gc_static_add(state, castobj(pl_static.str.sub));
    pylt_gc_static_add(state, castobj(pl_static.str.start));
    pylt_gc_static_add(state, castobj(pl_static.str.end));

    // int
    pylt_gc_static_add(state, castobj(pl_static.str.is_integer));

    // set
    pylt_gc_static_add(state, castobj(pl_static.str.add));
    pylt_gc_static_add(state, castobj(pl_static.str.clear));
    pylt_gc_static_add(state, castobj(pl_static.str.copy));
    pylt_gc_static_add(state, castobj(pl_static.str.pop));
    pylt_gc_static_add(state, castobj(pl_static.str.remove));

    // list
    pylt_gc_static_add(state, castobj(pl_static.str.append));
    pylt_gc_static_add(state, castobj(pl_static.str.count));
    pylt_gc_static_add(state, castobj(pl_static.str.index));
    pylt_gc_static_add(state, castobj(pl_static.str.extend));
    pylt_gc_static_add(state, castobj(pl_static.str.insert));
    pylt_gc_static_add(state, castobj(pl_static.str.reverse));

	pylt_gc_static_add(state, castobj(pl_static.str.None));
	pylt_gc_static_add(state, castobj(pl_static.str.True));
	pylt_gc_static_add(state, castobj(pl_static.str.False));

    pylt_gc_static_add(state, castobj(pl_static.str.math));
    pylt_gc_static_add(state, castobj(pl_static.str.builtins));

    pylt_gc_static_add(state, castobj(pl_static.str.cls));
    pylt_gc_static_add(state, castobj(pl_static.str.self));
    pylt_gc_static_add(state, castobj(pl_static.str.args));
    pylt_gc_static_add(state, castobj(pl_static.str.kwargs));
    pylt_gc_static_add(state, castobj(pl_static.str.param1));
    pylt_gc_static_add(state, castobj(pl_static.str.param2));
    pylt_gc_static_add(state, castobj(pl_static.str.param3));
    pylt_gc_static_add(state, castobj(pl_static.str.param4));

    pylt_gc_static_add(state, castobj(pl_static.str.x));
    pylt_gc_static_add(state, castobj(pl_static.str.y));

    pylt_gc_static_add(state, castobj(pl_static.str.BaseException));
    pylt_gc_static_add(state, castobj(pl_static.str.SystemExit));
    pylt_gc_static_add(state, castobj(pl_static.str.KeyboardInterrupt));
    pylt_gc_static_add(state, castobj(pl_static.str.GeneratorExit));
    pylt_gc_static_add(state, castobj(pl_static.str.Exception));
    pylt_gc_static_add(state, castobj(pl_static.str.StopIteration));
    pylt_gc_static_add(state, castobj(pl_static.str.StopAsyncIteration));
    pylt_gc_static_add(state, castobj(pl_static.str.ArithmeticError));
    pylt_gc_static_add(state, castobj(pl_static.str.FloatingPointError));
    pylt_gc_static_add(state, castobj(pl_static.str.OverflowError));
    pylt_gc_static_add(state, castobj(pl_static.str.ZeroDivisionError));
    pylt_gc_static_add(state, castobj(pl_static.str.AssertionError));
    pylt_gc_static_add(state, castobj(pl_static.str.AttributeError));
    pylt_gc_static_add(state, castobj(pl_static.str.BufferError));
    pylt_gc_static_add(state, castobj(pl_static.str.EOFError));
    pylt_gc_static_add(state, castobj(pl_static.str.ImportError));
    pylt_gc_static_add(state, castobj(pl_static.str.LookupError));
    pylt_gc_static_add(state, castobj(pl_static.str.IndexError));
    pylt_gc_static_add(state, castobj(pl_static.str.KeyError));
    pylt_gc_static_add(state, castobj(pl_static.str.MemoryError));
    pylt_gc_static_add(state, castobj(pl_static.str.NameError));
    pylt_gc_static_add(state, castobj(pl_static.str.UnboundLocalError));
    pylt_gc_static_add(state, castobj(pl_static.str.OSError));
    pylt_gc_static_add(state, castobj(pl_static.str.BlockingIOError));
    pylt_gc_static_add(state, castobj(pl_static.str.ChildProcessError));
    pylt_gc_static_add(state, castobj(pl_static.str.ConnectionError));
    pylt_gc_static_add(state, castobj(pl_static.str.BrokenPipeError));
    pylt_gc_static_add(state, castobj(pl_static.str.ConnectionAbortedError));
    pylt_gc_static_add(state, castobj(pl_static.str.ConnectionRefusedError));
    pylt_gc_static_add(state, castobj(pl_static.str.ConnectionResetError));
    pylt_gc_static_add(state, castobj(pl_static.str.FileExistsError));
    pylt_gc_static_add(state, castobj(pl_static.str.FileNotFoundError));
    pylt_gc_static_add(state, castobj(pl_static.str.InterruptedError));
    pylt_gc_static_add(state, castobj(pl_static.str.IsADirectoryError));
    pylt_gc_static_add(state, castobj(pl_static.str.NotADirectoryError));
    pylt_gc_static_add(state, castobj(pl_static.str.PermissionError));
    pylt_gc_static_add(state, castobj(pl_static.str.ProcessLookupError));
    pylt_gc_static_add(state, castobj(pl_static.str.TimeoutError));
    pylt_gc_static_add(state, castobj(pl_static.str.ReferenceError));
    pylt_gc_static_add(state, castobj(pl_static.str.RuntimeError));
    pylt_gc_static_add(state, castobj(pl_static.str.NotImplementedError));
    pylt_gc_static_add(state, castobj(pl_static.str.RecursionError));
    pylt_gc_static_add(state, castobj(pl_static.str.SyntaxError));
    pylt_gc_static_add(state, castobj(pl_static.str.IndentationError));
    pylt_gc_static_add(state, castobj(pl_static.str.TabError));
    pylt_gc_static_add(state, castobj(pl_static.str.SystemError));
    pylt_gc_static_add(state, castobj(pl_static.str.TypeError));
    pylt_gc_static_add(state, castobj(pl_static.str.ValueError));
    pylt_gc_static_add(state, castobj(pl_static.str.UnicodeError));
    pylt_gc_static_add(state, castobj(pl_static.str.UnicodeDecodeError));
    pylt_gc_static_add(state, castobj(pl_static.str.UnicodeEncodeError));
    pylt_gc_static_add(state, castobj(pl_static.str.UnicodeTranslateError));
    pylt_gc_static_add(state, castobj(pl_static.str.Warning));
    pylt_gc_static_add(state, castobj(pl_static.str.DeprecationWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.PendingDeprecationWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.RuntimeWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.SyntaxWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.UserWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.FutureWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.ImportWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.UnicodeWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.BytesWarning));
    pylt_gc_static_add(state, castobj(pl_static.str.ResourceWarning));

	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_OBJECT_TO_STR));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_CLASS_TO_STR));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_MODULE_TO_STR));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_FUNCTION_TO_STR));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_CFUNCTION_TO_STR));

	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_EMPTY_SET));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_EMPTY_LIST));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_EMPTY_TUPLE));
	pylt_gc_static_add(state, castobj(pl_static.str.TMPL_EMPTY_DICT));
}
