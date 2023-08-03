/*
An interface which handles LEX tokens.
*/

#include "tokenize.h"

static YYSTYPE _curr_token;
static int _la_exists;
static int _curr_token_type;

/* Initialises the tokens */
int tokenize_init()
{
    _la_exists = FALSE;
    return XSM_SUCCESS;
}

/* Returns next token */
int tokenize_next_token(YYSTYPE *token_info)
{
    int token_type;

    if (_la_exists)
    {
        *token_info = _curr_token;
        _la_exists = FALSE;
        return _curr_token_type;
    }
    else
    {
        token_type = yylex();
        *token_info = yylval;
        return token_type;
    }
}
/* Peeks the next token */
int tokenize_peek(YYSTYPE *token_info)
{
    if (_la_exists)
    {
        *token_info = _curr_token;
        return _curr_token_type;
    }
    else
    {
        _curr_token_type = yylex();
        _curr_token = yylval;
        *token_info = _curr_token;
        _la_exists = TRUE;
        return _curr_token_type;
    }
}

/* Skips the next token */
int tokenize_skip_token()
{
    YYSTYPE token_info;
    return tokenize_next_token(&token_info);
}

/* Closes the tokens */
int tokenize_close()
{
    return XSM_SUCCESS;
}

/* Resets the tokens */
void tokenize_reset()
{
    _la_exists = FALSE;
}

/* Clears the token stream */
void tokenize_clear_stream()
{
    lexer_buffer_reset();
}