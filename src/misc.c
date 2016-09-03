
#include "misc.h"
#include "intp.h"
#include "types/all.h"

void putcode(uint32_t code) {
    if (code < 0xff) {
        putchar((char)code);
    } else {
        char* ret = ucs4_to_utf8(code);
        printf_u8("%s", ret);
        free(ret);
    }
}

void raw_str_print(RawString *rs) {
    uint32_t code;
    for (const uint8_t *p = rs->s; p != rs->e;) {
        p = utf8_decode(p, &code);
        putcode(code);
    }
}


#define sstr_new(_name) pl_static.str._name = pylt_obj_str_new_from_cstr_static(I, #_name, true)
#define sstr_new2(_name, _str) pl_static.str._name = pylt_obj_str_new_from_cstr_static(I, (_str), true);

// replace
// [ \t]+PyLiteStrObject \*([A-Za-z0-9_]+);
// sstr_new\($1\);

// pl_static.str.([^ ]+) = pylt_obj_str_new_from_cstr_static\(I, "([^"]*)", true\);
// sstr_new2\($1, "$2"\);

void pylt_misc_static_objs_init(PyLiteInterpreter *I) {
	sstr_new(__base__);
	sstr_new(__call__);
	sstr_new(__new__);
    sstr_new(__import__);
    sstr_new(__init__);
    sstr_new(__str__);
    sstr_new(__repr__);

	sstr_new(__add__);
	sstr_new(__div__);
	sstr_new(__floordiv__);
	sstr_new(__mul__);
	sstr_new(__neg__);
	sstr_new(__pos__);
	sstr_new(__pow__);
	sstr_new(__sub__);

	sstr_new(__lshift__);
	sstr_new(__rshift__);

	sstr_new(__hash__);
	sstr_new(__iter__);
	sstr_new(__cmp__);
	sstr_new(__eq__);
	sstr_new(__setattr__);
	sstr_new(__getattr__);
	sstr_new(__setitem__);
	sstr_new(__getitem__);

	sstr_new(__defaults__);
    sstr_new(__parameters__);
    sstr_new(__args_types__);

    sstr_new(abs);
    sstr_new(all);
    sstr_new(any);
    sstr_new(ascii);
    sstr_new(bin);
    sstr_new(bytearray);
    sstr_new(callable);
    sstr_new(chr);
    sstr_new(classmethod);
    sstr_new(compile);
    sstr_new(delattr);
    sstr_new(dir);
    sstr_new(divmod);
    sstr_new(enumerate);
    sstr_new(eval);
    sstr_new(exec);
    sstr_new(filter);
    sstr_new(format);
    sstr_new(frozenset);
    sstr_new(getattr);
    sstr_new(globals);
    sstr_new(hasattr);
    sstr_new(hash);
    sstr_new(help);
    sstr_new(hex);
    sstr_new(id);
    sstr_new(input);
    sstr_new(isinstance);
    sstr_new(issubclass);
    sstr_new(iter);
    sstr_new(len);
    sstr_new(locals);
    sstr_new(map);
    sstr_new(max);
    sstr_new(memoryview);
    sstr_new(min);
    sstr_new(next);
    sstr_new(oct);
    sstr_new(open);
    sstr_new(ord);
    sstr_new(pow);
    sstr_new(print);
    sstr_new(repr);
    sstr_new(reversed);
    sstr_new(round);
    sstr_new(setattr);
    sstr_new(slice);
    sstr_new(sorted);
    sstr_new(staticmethod);
    sstr_new(sum);
    sstr_new(super);
    sstr_new(vars);
    sstr_new(zip);

    // classes
    sstr_new(object);
    sstr_new2(int_, "int");
    sstr_new2(float_, "float");
    sstr_new2(bool_, "bool");
    sstr_new(str);
    sstr_new(bytes);
    sstr_new(set);
    sstr_new(list);
    sstr_new(tuple);
    sstr_new(dict);
    sstr_new(unusual);

    sstr_new(module);
    sstr_new(function);
    sstr_new(cfunction);
    sstr_new(code);
    sstr_new(type);
    sstr_new(iterator);
    sstr_new2(property_, "property");
    sstr_new(NoneType);
    sstr_new(range);

    // object
    sstr_new(mro);

    // str
    sstr_new(join);
    sstr_new(sub);
    sstr_new(start);
    sstr_new(end);

    // int
    sstr_new(is_integer);

    // set
    sstr_new(add);
    sstr_new(clear);
    sstr_new(copy);
    sstr_new(pop);
    sstr_new(remove);

    // list
    sstr_new(append);
    sstr_new(count);
    sstr_new(index);
    sstr_new(extend);
    sstr_new(insert);
    sstr_new(reverse);

    // unusual
    sstr_new(unknown);
    sstr_new(useless);
    sstr_new(param_args);
    sstr_new(param_kwargs);
    sstr_new(param_undefined);

    // prop
    sstr_new(fget);
    sstr_new(fset);

    sstr_new(None);
    sstr_new(True);
    sstr_new(False);

    // modules
    sstr_new(math);
    sstr_new(builtins);

    sstr_new(cls);
    sstr_new(self);
    sstr_new(args);
    sstr_new(kwargs);
    sstr_new(param1);
    sstr_new(param2);
    sstr_new(param3);
    sstr_new(param4);
    sstr_new2(default_, "default");

    sstr_new(x);
    sstr_new(y);

    // exceptions
    sstr_new(BaseException);
    sstr_new(SystemExit);
    sstr_new(KeyboardInterrupt);
    sstr_new(GeneratorExit);
    sstr_new(Exception);
    sstr_new(StopIteration);
    sstr_new(StopAsyncIteration);
    sstr_new(ArithmeticError);
    sstr_new(FloatingPointError);
    sstr_new(OverflowError);
    sstr_new(ZeroDivisionError);
    sstr_new(AssertionError);
    sstr_new(AttributeError);
    sstr_new(BufferError);
    sstr_new(EOFError);
    sstr_new(ImportError);
    sstr_new(LookupError);
    sstr_new(IndexError);
    sstr_new(KeyError);
    sstr_new(MemoryError);
    sstr_new(NameError);
    sstr_new(UnboundLocalError);
    sstr_new(OSError);
    sstr_new(BlockingIOError);
    sstr_new(ChildProcessError);
    sstr_new(ConnectionError);
    sstr_new(BrokenPipeError);
    sstr_new(ConnectionAbortedError);
    sstr_new(ConnectionRefusedError);
    sstr_new(ConnectionResetError);
    sstr_new(FileExistsError);
    sstr_new(FileNotFoundError);
    sstr_new(InterruptedError);
    sstr_new(IsADirectoryError);
    sstr_new(NotADirectoryError);
    sstr_new(PermissionError);
    sstr_new(ProcessLookupError);
    sstr_new(TimeoutError);
    sstr_new(ReferenceError);
    sstr_new(RuntimeError);
    sstr_new(NotImplementedError);
    sstr_new(RecursionError);
    sstr_new(SyntaxError);
    sstr_new(IndentationError);
    sstr_new(TabError);
    sstr_new(SystemError);
    sstr_new(TypeError);
    sstr_new(ValueError);
    sstr_new(UnicodeError);
    sstr_new(UnicodeDecodeError);
    sstr_new(UnicodeEncodeError);
    sstr_new(UnicodeTranslateError);
    sstr_new(Warning);
    sstr_new(DeprecationWarning);
    sstr_new(PendingDeprecationWarning);
    sstr_new(RuntimeWarning);
    sstr_new(SyntaxWarning);
    sstr_new(UserWarning);
    sstr_new(FutureWarning);
    sstr_new(ImportWarning);
    sstr_new(UnicodeWarning);
    sstr_new(BytesWarning);
    sstr_new(ResourceWarning);

    // template
    sstr_new2(TMPL_OBJECT_TO_STR, "<%s object at %p>");
    sstr_new2(TMPL_CLASS_TO_STR, "<class '%s'>");
    sstr_new2(TMPL_MODULE_TO_STR, "<module '%s'>");
    sstr_new2(TMPL_FUNCTION_TO_STR, "<function %s at %p>");
    sstr_new2(TMPL_CFUNCTION_TO_STR, "<cfunction %s at %p>");

    sstr_new2(TMPL_EMPTY_STR, "");
    sstr_new2(TMPL_EMPTY_SET, "set()");
    sstr_new2(TMPL_EMPTY_LIST, "[]");
    sstr_new2(TMPL_EMPTY_TUPLE, "()");
    sstr_new2(TMPL_EMPTY_DICT, "{}");
}
