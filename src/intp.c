
#include "intp.h"
#include "misc.h"

void pylt_intp_err(PyLiteInterpreter *I) {
    exit(-1);
}

PyLiteInterpreter* pylt_intp_new() {
    PyLiteInterpreter *I = pylt_realloc(NULL, sizeof(PyLiteInterpreter));
    pylt_intp_init(I);
    return I;
}

void pylt_intp_free(PyLiteInterpreter *I) {
    pylt_intp_finalize(I);
    pylt_free(I);
}

void pylt_intp_init(PyLiteInterpreter *I) {
    kv_init(I->cls_base);
    I->modules = pylt_obj_dict_new(I);
    I->error_code = 0;

    I->lexer = pylt_realloc(NULL, sizeof(LexState));
    I->parser = pylt_realloc(NULL, sizeof(ParserState));

    pylt_gc_init(I);
    pylt_lex_init(I, I->lexer, NULL);
    pylt_parser_init(I, I->parser, I->lexer);
    pylt_misc_static_objs_init(I);
    pylt_vm_init(I, &I->vm);
}

void pylt_intp_finalize(PyLiteInterpreter *I) {
    pylt_gc_collect(I);
    pylt_lex_finalize(I, I->lexer);
    pylt_parser_finalize(I, I->parser);
    pylt_vm_finalize(I);

    // free types
    /*for (pl_uint_t i = 1; i < kv_size(interpreter->cls_base); ++i) {
        pylt_obj_type_free(interpreter, kv_A(interpreter->cls_base, i));
    }*/
    kv_destroy(I->cls_base);

    // free modules
    /*PyLiteDictObject *dict = I->modules;
    for (pl_int32_t it = pylt_obj_dict_begin(I, dict); it != pylt_obj_dict_end(I, dict); pylt_obj_dict_next(I, dict, &it)) {
        PyLiteObject *k, *v;
        pylt_obj_dict_keyvalue(I, dict, it, &k, &v);
        pylt_obj_free(I, k);
        pylt_obj_free(I, v);
    }*/
 
    //pylt_obj_dict_free(I, I->modules);
    pylt_gc_freeall(I);
    pylt_gc_finalize(I);

    pylt_free(I->lexer);
    pylt_free(I->parser);
}

void pylt_intp_load_stream(PyLiteInterpreter *I, StringStream *ss) {
    PyLiteCodeObject *code;
    I->lexer->ss = ss;
    code = pylt_parser_parse(I->parser);
    pylt_obj_code_add_to_gc(I, code);
    pylt_vm_load_code(I, code);
}

void pylt_intp_run(PyLiteInterpreter *I) {
    pylt_vm_run(I, NULL);
}
