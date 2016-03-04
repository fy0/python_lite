
#include "builtin.h"
#include "../state.h"
#include "../debug.h"
#include "helper.h"

void pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject *args);


void pylt_mods_builtins_print(PyLiteState *state, int argc, PyLiteObject *args) {
    PyLiteObject *obj;
    if (argc) {
        for (int i = 0; i < argc; i++) {
            obj = pylt_state_obj_get(state, argc, i);
            debug_print_obj(obj); 
            if (i != argc - 1) printf(" ");
        }
    }
    putchar('\n');
}

void pylt_mods_builtins_register(PyLiteState *state) {

    pylt_func_bind(
        _NS(state, "print"),
        _NST(state, 3, "values", "sep", "end"),
        _NT(state, 3, PARAM_NODEF, _NS(state, " "), _NS(state, "\n")),
        NULL, 
        &pylt_mods_builtins_print
    );

}
