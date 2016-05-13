
#include "state.h"

void pylt_state_err(PyLiteState *state) {
    exit(-1);
}

void pylt_state_init(PyLiteState *state) {
    kv_init(state->cls_base);
    state->modules = pylt_obj_dict_new(state);
    state->cache_str = pylt_obj_set_new(state);
    state->cache_bytes = pylt_obj_set_new(state);
    state->error_code = 0;

    state->lexer = pylt_realloc(NULL, sizeof(LexState));
    state->parser = pylt_realloc(NULL, sizeof(ParserState));

    pylt_lex_init(state, state->lexer, NULL);
    pylt_parser_init(state, state->parser, state->lexer);
    pylt_utils_static_objs_init(state);
    pylt_vm_init(state, &state->vm);
}

void pylt_state_load_stream(PyLiteState *state, StringStream *ss) {
    state->lexer->ss = ss;
    parse(state->parser);
}

void pylt_state_run(PyLiteState *state) {
    pylt_vm_run(state, state->parser->info->code);
}

void pylt_utils_static_objs_init(PyLiteState *state) {
    pl_static.str.__init__ = pylt_obj_str_new_from_c_str(state, "__init__", true);
    pl_static.str.__new__ = pylt_obj_str_new_from_c_str(state, "__new__", true);
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
    pl_static.str.__cmp__ = pylt_obj_str_new_from_c_str(state, "__cmp__", true);
    pl_static.str.__setattr__ = pylt_obj_str_new_from_c_str(state, "__setattr__", true);
    pl_static.str.__getattr__ = pylt_obj_str_new_from_c_str(state, "__getattr__", true);

    pl_static.str.id = pylt_obj_str_new_from_c_str(state, "id", true);
    pl_static.str.dir = pylt_obj_str_new_from_c_str(state, "dir", true);
    pl_static.str.len = pylt_obj_str_new_from_c_str(state, "len", true);
    pl_static.str.hash = pylt_obj_str_new_from_c_str(state, "hash", true);
    pl_static.str.iter = pylt_obj_str_new_from_c_str(state, "iter", true);
    pl_static.str.super = pylt_obj_str_new_from_c_str(state, "super", true);
    pl_static.str.isinstance = pylt_obj_str_new_from_c_str(state, "isinstance", true);
    pl_static.str.print = pylt_obj_str_new_from_c_str(state, "print", true);
    pl_static.str.setattr = pylt_obj_str_new_from_c_str(state, "setattr", true);
    pl_static.str.getattr = pylt_obj_str_new_from_c_str(state, "getattr", true);
    pl_static.str.range = pylt_obj_str_new_from_c_str(state, "range", true);

    pl_static.str.object = pylt_obj_str_new_from_c_str(state, "object", true);
    pl_static.str.int_ = pylt_obj_str_new_from_c_str(state, "int_", true);
    pl_static.str.float_ = pylt_obj_str_new_from_c_str(state, "float_", true);
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

    pl_static.str.param1 = pylt_obj_str_new_from_c_str(state, "param1", true);
    pl_static.str.param2 = pylt_obj_str_new_from_c_str(state, "param2", true);
    pl_static.str.param3 = pylt_obj_str_new_from_c_str(state, "param3", true);
    pl_static.str.param4 = pylt_obj_str_new_from_c_str(state, "param4", true);
}
