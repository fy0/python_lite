
#include "intp.h"
#include "utils/misc.h"
#include "utils/fs.h"
#include "utils/io.h"
#include "utils/io_simple.h"
#include "mods/pltypes.h"
#include "mods/io.h"
#include "mods/os.h"
#include "mods/sys.h"
#include "mods/cio.h"
#include "mods/math.h"
#include "mods/builtin.h"
#include "mods/unusualm.h"

void pylt_intp_err(PyLiteInterpreter *I) {
    exit(-1);
}

PyLiteInterpreter* pylt_intp_new() {
    PyLiteInterpreter *I = pylt_malloc(NULL, sizeof(PyLiteInterpreter));
    pylt_intp_init(I);
    return I;
}

void pylt_intp_free(PyLiteInterpreter *I) {
    pylt_intp_finalize(I);
    pylt_free_ex(NULL, I);
}

void pylt_intp_sys_init(PyLiteInterpreter *I) {
    pylt_io_init(I);
}

void pylt_intp_init(PyLiteInterpreter *I) {
    I->mem_used = 0;
    pylt_intp_sys_init(I);
    pylt_gc_init(I); 

    kv_init(I, I->cls_base);
    I->modules = pylt_obj_dict_new(I);
    I->cmodules_loader = pylt_obj_dict_new(I);
    I->error = NULL;

    I->lexer = NULL;
    I->parser = NULL;

    pylt_static_objs_init(I);

    // register builtins
    pylt_mods_pltypes_register(I);
    pylt_mods_io_register(I);
    pylt_mods_cio_register(I);
    pylt_mods_math_register(I);
    pylt_mods_os_register(I);
    pylt_mods_sys_register(I);
    pylt_mods_unusual_register(I);

    pylt_mods_builtins_register(I);
    pylt_vm_init(I, &I->vm);
}

void pylt_intp_finalize(PyLiteInterpreter *I) {
    pylt_gc_collect(I);
    if (I->lexer) pylt_lex_finalize(I, I->lexer);
    if (I->parser) pylt_parser_finalize(I, I->parser);
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

    if (I->lexer) pylt_free_ex(I, I->lexer);
    if (I->parser) pylt_free_ex(I, I->parser);
}

void pylt_intp_loadf(PyLiteInterpreter *I, PyLiteFile *input) {
    PyLiteCodeObject *code = pylt_intp_parsef(I, input);
    pylt_vm_load_code(I, code);
}

PyLiteCodeObject* pylt_intp_parsef(PyLiteInterpreter *I, PyLiteFile *input) {
    if (!I->lexer) I->lexer = pylt_lex_new(I, input);
    else pylt_lex_reset(I->lexer, input);
    if (!I->parser) I->parser = pylt_parser_new(I, I->lexer);
    else pylt_parser_reset(I, I->parser, I->lexer);

    PyLiteCodeObject *code = pylt_parser_parse(I->parser);
    pylt_obj_code_add_to_gc(I, code);
    return code;
}
