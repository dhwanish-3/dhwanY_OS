/*
Interface that handles registers.
*/

#include "data.h"

/* Checks if the register can be used by SPL */
int isAllowedRegister(int value)
{
    if (value >= R0 && value < R0 + C_REG_BASE)
        return TRUE;
    return FALSE;
}