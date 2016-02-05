
#ifndef PYLITE_PARSER_H
#define PYLITE_PARSER_H

#include "lexer.h"

typedef struct ParserState {
    LexState *ls;
} ParserState;

void parse(ParserState *ps);

#endif
