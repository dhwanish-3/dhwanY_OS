#ifndef XSM_EXCEPTION_H

#define XSM_EXCEPTION_H

#include "types.h"

#define EXP_PAGEFAULT 0
#define EXP_ILLINSTR 1
#define EXP_ILLMEM 2
#define EXP_ARITH 3

typedef struct _xsm_exception
{
    char *message;
    int type;
    int mode;
    int ma;
    int epn;
} xsm_exception;

int exception_set(char *message, int type, int mode);
char *exception_message();
int exception_code();
void exception_set_ma(int address);
void exception_set_epn(int page);
int exception_get_ma();
int exception_get_epn();

#endif