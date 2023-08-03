/*
An interface for handling registers.
*/

#include "registers.h"

#include <stdlib.h>
#include <string.h>

static xsm_reg *_registers;

static const char *_register_names[] = {
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "R13",
    "R14",
    "R15",
    "R16",
    "R17",
    "R18",
    "R19",

    "P0",
    "P1",
    "P2",
    "P3",

    "BP",
    "SP",
    "IP",

    "PTBR",
    "PTLR",
    "EIP",
    "EC",
    "EPN",
    "EMA"};

/* Initialise the registers */
int registers_init()
{
    _registers = (xsm_reg *)malloc(sizeof(xsm_reg) * XSM_NUM_REG);

    if (!_registers)
        return XSM_FAILURE;

    return XSM_SUCCESS;
}

/* Returns the register code for the given register name */
int registers_get_register_code(const char *name)
{
    int i;

    for (i = 0; i < XSM_NUM_REG; i++)
        if (!strcasecmp(name, _register_names[i]))
            return i;

    return -1;
}

/* Returns the register for the given register name */
xsm_reg *registers_get_register(const char *name)
{
    int code = registers_get_register_code(name);

    if (code > -1)
        return &_registers[code];

    return NULL;
}

/* Deallocates the registers */
void registers_destroy()
{
    free(_registers);
}

/* Returns the register names */
const char **registers_names()
{
    return _register_names;
}

/* Returns the number of registers */
int registers_len()
{
    return XSM_NUM_REG;
}

/* Returns the integer value stored in the given register */
int registers_get_integer(const char *name)
{
    xsm_word *reg = registers_get_register(name);
    return word_get_integer(reg);
}

/* Returns the string value stored in the given register */
char *registers_get_string(const char *name)
{
    xsm_word *reg = registers_get_register(name);

    if (!reg)
        return NULL;

    return word_get_string(reg);
}

/* Stores the integer value in the given register */
int registers_store_integer(const char *name, int val)
{
    xsm_word *reg = registers_get_register(name);
    return word_store_integer(reg, val);
}

/* Stores the string value in the given register */
int registers_store_string(const char *name, char *str)
{
    xsm_word *reg = registers_get_register(name);
    return word_store_string(reg, str);
}

/* Checks whether the given register can be used in USER mode */
int registers_umode(const char *reg)
{
    int code = registers_get_register_code(reg);

    if (code < 0)
        return FALSE;

    if (code >= REG_PORT_LOW && code <= REG_PORT_HIGH)
        return FALSE;

    if (code >= REG_KERN_LOW && code <= REG_KERN_LOW)
        return FALSE;

    return TRUE;
}