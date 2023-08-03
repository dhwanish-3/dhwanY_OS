/*
Interface to handle exceptions.
*/

#include "exception.h"

#include <stdio.h>

extern jmp_buf exp_point;

/* Throws exception */
void exception_throwException(int code)
{
    longjmp(exp_point, code);
}

/* Prints error message */
void exception_printErrorMessage(int code)
{
    switch (code)
    {
    case EXCEPTION_CANT_OPEN_DISK:
        printf("Unable to open disk file\n");
        break;

    case EXCEPTION_CANT_CREATE_DISK:
        printf("Failed to create disk file\n");
        break;
    }
}