#ifndef XSM_TOKENIZE_H

#define XSM_TOKENIZE_H

#include "lexer.h"
#include "types.h"

int tokenize_init();
int tokenize_next_token(YYSTYPE *token_info);
int tokenize_peek(YYSTYPE *token_info);
int tokenize_skip_token();
int tokenize_close();
void tokenize_reset();
void tokenize_clear_stream();

int yylex();

#endif