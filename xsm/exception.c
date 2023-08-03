/*
An interface for handling exceptions.
*/

#include "exception.h"

#include <stdio.h>

static xsm_exception _exception;

/* Set the exception variables */
int exception_set(char *message, int type, int mode)
{
    _exception.message = message;
    _exception.type = type;
    _exception.mode = mode;

    return XSM_SUCCESS;
}

/* Retrieve the exception message */
char *exception_message()
{
    return _exception.message;
}

/* Retrieve the exception type */
int exception_code()
{
    return _exception.type;
}

/* Set memory address */
void exception_set_ma(int address)
{
    _exception.ma = address;
}

/* Set exception page number */
void exception_set_epn(int page)
{
    _exception.epn = page;
}

/* Retrieve memory address */
int exception_get_ma()
{
    return _exception.ma;
}

/* Retrieve exception page number */
int exception_get_epn()
{
    return _exception.epn;
}