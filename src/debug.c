
#include "debug.h"
#include "vm.h"

void debug_print_const_vals(ParserState *ps) {
    printf("CONST VALS:\n");
    for (int i = 0; i < kv_size(ps->const_val); i++) {
        PyLiteObject *obj = kv_A(ps->const_val, i);
        printf("%3d %3d\n", i, obj->ob_type);
    }
}

void debug_print_opcodes(ParserState *ps) {
    printf("OPCODES:\n");
    for (int i = 0; i < kv_size(ps->opcodes); i++) {
        switch (kv_A(ps->opcodes, i)) {
            case BC_LOADCONST:
                printf("%15s %d\n", "LOADCONST", kv_A(ps->opcodes, ++i));
                break;
            case BC_OPERATOR:
                printf("%15s %d\n", "OPERATOR", kv_A(ps->opcodes, ++i));
                break;
        }
    }
}
