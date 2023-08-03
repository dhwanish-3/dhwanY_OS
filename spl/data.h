#ifndef SPL_DATA_H

#define SPL_DATA_H

#include <stdbool.h>

#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15
#define R16 16
#define R17 17
#define R18 18
#define R19 19

#define P0 20
#define P1 21
#define P2 22
#define P3 23

#define BP 24
#define IP 25
#define SP 26
#define PTBR 27
#define PTLR 28
#define EIP 29
#define EPN 30
#define EC 31
#define EMA 32

#define NUM_GEN_REG 20
#define NUM_PORTS 4
#define NUM_SPECIAL_REG 9

#define NUM_REGS (NO_GEN_REG + NO_SPECIAL_REG + NO_PORTS)

// For compilers use only
#define C_REG_BASE 16
#define REG_NAME_MAX_LEN 5

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

int isAllowedRegister(int value);

#endif