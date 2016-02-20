
#include "state.h"
#include "lexer.h"
#include "types/number.h"


_INLINE static uint8_t _hex(uint32_t code) {
    if (code >= '0' && code <= '9') return code - '0';
    else if (code >= 'A' && code <= 'F') return code - 'A' + 10;
    else if (code >= 'a' && code <= 'f') return code - 'a' + 10;
    return 255;
}

_INLINE static int8_t _oct(uint32_t code) {
    if (code >= '0' && code <= '7') return code - '0';
    return 255;
}

_INLINE static uint8_t _bin(uint32_t code) {
    if (code >= '0' && code <= '1') return code - '0';
    return 255;
}

_INLINE static uint8_t _dec(uint32_t code) {
    if (code >= '0' && code <= '9') return code - '0';
    return 255;
}


int _read_x_int(RawString *str, int n, uint8_t(*func)(uint32_t code), int max_size) {
    const uint8_t *p = str->s;
    const uint8_t *e = (max_size > 0) ? str->s + max_size : str->e;
    int ret = 0, val = (int)pow(n, e - p - 1);

    do {
        ret += (*func)(*p++) * val;
        val /= n;
    } while (p != e);

    return ret;
}


double _read_float(RawString *str, int start_offset) {
    const uint8_t *p = str->s + start_offset;
    const uint8_t *e = str->e;
    double ret = 0, val = 0.1;

    do {
        ret += _dec(*p++) * val;
        val /= 10;
    } while (p != e);

    return ret;
}


PyLiteObject* new_obj_number_from_token(PyLiteState *state, Token *tk) {
    double fret;
    uint32_t iret; 

    switch (tk->val) {
        case TK_INT:
            switch (tk->extra) {
                case 0: iret = _read_x_int(&(tk->str), 10, _dec, 0); break;
                case 1: iret = _read_x_int(&(tk->str), 16, _hex, 0); break;
                case 2: iret = _read_x_int(&(tk->str),  2, _bin, 0); break;
                case 3: iret = _read_x_int(&(tk->str),  8, _oct, 0); break;
            }
            return castobj(pylt_obj_int_new(state, iret));
        case TK_FLOAT:
            iret = _read_x_int(&(tk->str), 10, _dec, tk->extra-1);
            fret = _read_float(&(tk->str), tk->extra);
            return castobj(pylt_obj_float_new(state, fret + iret));
    }
    return NULL;
}

PyLiteObject* new_obj_bytes_from_token(PyLiteState *state, Token *tk) {
    ;
}