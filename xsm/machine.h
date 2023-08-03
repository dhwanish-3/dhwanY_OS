#ifndef XSM_MACHINE_H

#define XSM_MACHINE_H

#include <setjmp.h>

#include "debug.h"
#include "disk.h"
#include "exception.h"
#include "memory.h"
#include "registers.h"
#include "tokenize.h"
#include "types.h"

#define XSM_ADDR_DREF 0
#define XSM_ADDR_NODREF 1

#define PRIVILEGE_USER 0
#define PRIVILEGE_KERNEL 1

#define XSM_EXCEPTION_OCCURED -1

/* Operation codes. */
#define MOV 0
#define ADD 1
#define SUB 2
#define MUL 3
#define DIV 4
#define MOD 5
#define INR 6
#define DCR 7
#define LT 8
#define GT 9
#define EQ 10
#define NE 11
#define GE 12
#define LE 13
#define JZ 14
#define JNZ 15
#define JMP 16
#define PUSH 17
#define POP 18
#define CALL 19
#define RET 20
#define BRKP 21
#define INT 22

#define LOADI 23
#define LOAD 24
#define STORE 25
#define ENCRYPT 26
#define BACKUP 27
#define RESTORE 28
#define PORT 29
#define IN 30
#define INI 31
#define OUT 32
#define IRET 33
#define HALT 34
#define NOP 35

/* Between these values are the privileged instructions. */
#define TOKEN_KERN_LOW 23
#define TOKEN_KERN_HIGH 34

#define INTERRUPT_LOW 4
#define INTERRUPT_HIGH 18

#define XSM_INSTRUCTION_COUNT 36

#define XSM_DISKOP_LOAD 0
#define XSM_DISKOP_STORE 1

#define XSM_CONSOLE_PRINT 0
#define XSM_CONSOLE_READ 1

#define XSM_INTERRUPT_EXHANDLER 0
#define XSM_HALT -1

typedef struct _disk_operation
{
    int src_block;
    int dest_page;
    int operation;
} disk_operation;

typedef struct _console_operation
{
    xsm_word word;
    int operation;
} console_operation;

typedef struct _xsm_cpu
{
    xsm_reg *regs;
    int timer;
    int mode;
    int disk_state, disk_wait;
    int console_state, console_wait;

    int mem_left, mem_right;

    disk_operation disk_op;
    console_operation console_op;

    /* Exception point */
    jmp_buf h_exp_point;
} xsm_cpu;

typedef struct _xsm_options
{
    int timer;
    int debug;
    int disk;
    int console;
} xsm_options;

int machine_init(xsm_options *options);
int machine_get_opcode(const char *instr);
xsm_word *machine_get_ipreg();
xsm_word *machine_get_spreg();
xsm_word *machine_get_register(const char *name);
int machine_instr_req_privilege(int opcode);
int machine_serve_instruction(char *buffer, unsigned long *read_bytes, int max);
int machine_run();
void machine_register_exception(char *message, int code);
int machine_handle_exception();
void machine_get_mem_access(int *mem_left, int *mem_right);
void machine_pre_execute(int ip_val);
void machine_post_execute();
int machine_execute_instruction(int opcode);
xsm_word *machine_get_address(int write);
int machine_get_address_int(int write);
int machine_translate_address(int address, int write, int type, int mode);
xsm_word *machine_memory_get_word(int address);
int machine_execute_mov();
int machine_execute_arith(int opcode);
int machine_execute_unary(int opcode);
int machine_execute_logical(int opcode);
int machine_execute_jump(int opcode);
int machine_execute_stack(int opcode);
int machine_push_do(xsm_word *reg);
int machine_pop_do(xsm_word *dest);
xsm_word *machine_stack_pointer(int write);
int machine_execute_call_do(int target);
int machine_execute_call();
int machine_execute_ret();
int machine_execute_brkp();
int machine_execute_interrupt();
int machine_execute_interrupt_do(int interrupt);
int machine_interrupt_address(int int_num);
int machine_execute_disk(int operation, int immediate);
int machine_read_disk_arg();
int machine_schedule_disk(int page_num, int block_num, int firetime, int operation);
int machine_execute_load_do(int page_num, int block_num);
int machine_execute_store_do(int page_num, int block_num);
int machine_execute_encrypt();
int machine_execute_backup();
int machine_execute_restore();
int machine_execute_print_do(xsm_word *word);
int machine_execute_print();
int machine_schedule_in(int firetime);
int machine_execute_ini();
int machine_execute_in_do(xsm_word *word);
int machine_execute_iret();
int machine_get_mode();
void machine_set_mode(int mode);
void machine_destroy();

#endif