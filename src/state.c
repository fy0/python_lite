
#include "state.h"
#include "misc.h"

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
    state->error_code = 0;

    state->lexer = pylt_realloc(NULL, sizeof(LexState));
    state->parser = pylt_realloc(NULL, sizeof(ParserState));

    pylt_gc_init(state);
    pylt_lex_init(state, state->lexer, NULL);
    pylt_parser_init(state, state->parser, state->lexer);
    pylt_misc_static_objs_init(state);
    pylt_vm_init(state, &state->vm);
}

void pylt_state_finalize(PyLiteState *state) {
    pylt_gc_collect(state);
    pylt_lex_finalize(state, state->lexer);
    pylt_parser_finalize(state, state->parser);
    pylt_vm_finalize(state);

    // free types
    /*for (pl_uint_t i = 1; i < kv_size(state->cls_base); ++i) {
        pylt_obj_type_free(state, kv_A(state->cls_base, i));
    }*/
    kv_destroy(state->cls_base);

    // free modules
    /*PyLiteDictObject *dict = state->modules;
    for (pl_int_t it = pylt_obj_dict_begin(state, dict); it != pylt_obj_dict_end(state, dict); pylt_obj_dict_next(state, dict, &it)) {
        PyLiteObject *k, *v;
        pylt_obj_dict_keyvalue(state, dict, it, &k, &v);
        pylt_obj_free(state, k);
        pylt_obj_free(state, v);
    }*/
 
    //pylt_obj_dict_free(state, state->modules);
    pylt_gc_freeall(state);
    pylt_gc_finalize(state);

    pylt_free(state->lexer);
    pylt_free(state->parser);
}

void pylt_state_load_stream(PyLiteState *state, StringStream *ss) {
    PyLiteCodeObject *code;
    state->lexer->ss = ss;
    code = pylt_parser_parse(state->parser);
    pylt_vm_load_code(state, code);
}

void pylt_state_run(PyLiteState *state) {
    pylt_vm_run(state, NULL);
}
