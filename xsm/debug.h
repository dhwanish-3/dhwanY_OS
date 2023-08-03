#ifndef XSM_DEBUG_H

#define XSM_DEBUG_H

#include "machine.h"
#include "constants.h"

#define OFF FALSE
#define ON TRUE

#define DEBUG_COMMAND_LEN 100
#define DEBUG_STRING_LEN 100

#define DEBUG_STEP 0
#define DEBUG_CONTINUE 1
#define DEBUG_REG 2
#define DEBUG_MEM 3
#define DEBUG_PCB 4
#define DEBUG_PAGETABLE 5
#define DEBUG_DISKMAPTABLE 6
#define DEBUG_RESOURCETABLE 7
#define DEBUG_FILETABLE 8
#define DEBUG_SEMTABLE 9
#define DEBUG_MEMFREELIST 10
#define DEBUG_FILESTATUS 11
#define DEBUG_DISKSTATUS 12
#define DEBUG_SYSTEMSTATUS 13
#define DEBUG_TERMINALSTATUS 14
#define DEBUG_BUFFERTABLE 15
#define DEBUG_INODETABLE 16
#define DEBUG_USERTABLE 17
#define DEBUG_DISKFREELIST 18
#define DEBUG_ROOTFILE 19
#define DEBUG_LOCATION 20
#define DEBUG_VAL 21
#define DEBUG_WATCH 22
#define DEBUG_WATCHCLEAR 23
#define DEBUG_LIST 24
#define DEBUG_PAGE 25
#define DEBUG_EXIT 26
#define DEBUG_HELP 27

#define DEBUG_COUNT 28

#define DEBUG_LOC_PT 28672
#define MAX_PROC_NUM 16
#define PT_ENTRY_SIZE 16
#define DEBUG_PROC_RUNNING 2
#define MAX_NUM_PAGES 10
#define PTBR_PCB_OFFSET 14
#define DEBUG_PT_BASE 29696
#define RESOURCE_OFFSET 496
#define DEBUG_LIST_LEN 10

#define DEBUG_LOC_FILETABLE 28928
#define DEBUG_LOC_SEMTABLE 29056
#define DEBUG_LOC_FILESTATUS 29312
#define DEBUG_LOC_DISKSTATUS 29552
#define DEBUG_LOC_SYSTEMSTATUS 29560
#define DEBUG_LOC_TERMINALSTATUS 29568
#define DEBUG_LOC_BUFFERTABLE 30016
#define DEBUG_LOC_DISKMAPTABLE 30032
#define DEBUG_LOC_ROOTFILE 31744
#define DEBUG_LOC_MFL 29184
#define DEBUG_LOC_DFL 31232
#define DEBUG_LOC_INODE 30208
#define DEBUG_LOC_USERTABLE 31168

#define MAX_OPENFILE_NUM 32
#define MAX_FILE_NUM 60
#define MAX_USER_NUM 16
#define MAX_SEM_COUNT 32
#define MAX_MEM_PAGE 128
#define MAX_BUFFER 4
#define MAX_RESOURCE 8

#define DEBUG_MAX_WP 16
#define DEBUG_ERROR -1

struct _xsm_cpu;

typedef struct _xsm_cpu xsm_cpu;

typedef struct _debug_status
{
    int state;
    int prev_ip, ip;
    int prev_mode;
    int skip;
    int skip_command;
    int wp[16];
    int wp_size;
    char command[DEBUG_COMMAND_LEN];
} debug_status;

int debug_init();
void debug_activate();
void debug_deactivate();
int debug_next_step(int curr_ip);
int debug_show_interface();
int debug_command(char *command);
int debug_command_code(const char *cmd);
void debug_invalid_cmd(const char *cmd);
int debug_active_process();
int debug_skip_n(int num, int debug_command);
int debug_display_all_registers();
int debug_display_register(const char *regname);
int debug_display_range_reg(const char *reg_b_name, const char *reg_e_name);
int debug_display_mem(int page);
int debug_display_mem_range(int page_l, int page_h);
int debug_display_fields(int baseptr, const char **fields, const int *fields_len, int n_fields);
int debug_display_pcb_pid(int pid);
int debug_display_pcb();
int debug_display_pt_ptbr();
int debug_display_pt_at(int addr);
int debug_display_pt_pid(int pid);
int debug_display_dmt();
int debug_dmt_pid(int pid);
int debug_display_rt();
int debug_rt_pid(int pid);
int debug_display_ft();
int debug_display_st();
int debug_display_memlist();
int debug_display_fst();
int debug_display_dst();
int debug_display_sst();
int debug_display_tst();
int debug_display_bt();
int debug_display_inodetable();
int debug_display_usertable();
int debug_display_dfl();
int debug_display_rf();
int debug_display_location(int loc);
int debug_display_val(char *mem);
int debug_watch_add(int loc);
void debug_watch_clear();
int debug_watch_test(int mem_min, int mem_max);
int debug_display_list();
int debug_display_page(int ip);
void debug_display_help();

#endif