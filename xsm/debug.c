/*
The XSM debugger.
*/

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static debug_status _db_status;

const char *_db_commands_lh[] = {
    "step",
    "continue",
    "reg",
    "mem",
    "pcb",
    "pagetable",
    "diskmaptable",
    "resourcetable",
    "filetable",
    "semtable",
    "memfreelist",
    "filestatus",
    "diskstatus",
    "systemstatus",
    "terminalstatus",
    "buffertable",
    "inodetable",
    "usertable",
    "diskfreelist",
    "rootfile",
    "location",
    "val",
    "watch",
    "watchclear",
    "list",
    "page",
    "exit",
    "help"};

const char *_db_commands_sh[] = {
    "s",
    "c",
    "r",
    "m",
    "p",
    "pt",
    "dmt",
    "rt",
    "ft",
    "st",
    "mf",
    "fst",
    "dst",
    "sst",
    "tst",
    "bt",
    "it",
    "ut",
    "df",
    "rf",
    "loc",
    "v",
    "w",
    "wc",
    "l",
    "pg",
    "e",
    "h"};

/* Initialise debugger */
int debug_init()
{
    _db_status.state = OFF;
    _db_status.ip = -1;
    _db_status.prev_mode = PRIVILEGE_KERNEL;
    _db_status.skip = 0;
    strcpy(_db_status.command, "help");

    debug_watch_clear();

    return TRUE;
}

/* Activate the debugger */
void debug_activate()
{
    _db_status.state = ON;
}

/* Deactivate debugger */
void debug_deactivate()
{
    _db_status.state = OFF;
}

/* Called from machine for debugger */
int debug_next_step(int curr_ip)
{
    int mem_left, mem_right;
    int wp = DEBUG_ERROR;

    _db_status.prev_ip = _db_status.ip;
    _db_status.ip = curr_ip;

    machine_get_mem_access(&mem_left, &mem_right);

    if (mem_left > 0)
        wp = debug_watch_test(mem_left, mem_right);

    if (wp >= 0)
    {
        printf("Watchpoint at %d has been triggered.\n", _db_status.wp[wp]);
        _db_status.state = ON;
    }

    if (_db_status.state == ON)
        debug_show_interface();

    _db_status.prev_mode = machine_get_mode();

    return TRUE;
}

/* Display debugger interface */
int debug_show_interface()
{
    int done = FALSE, addr;
    char command[DEBUG_COMMAND_LEN], prev_instr[DEBUG_STRING_LEN], next_instr[DEBUG_STRING_LEN];

    if (_db_status.skip > 0)
    {
        _db_status.skip--;
        if (_db_status.skip_command == DEBUG_CONTINUE)
            debug_deactivate();
        return TRUE;
    }

    // Get the previous instruction
    addr = machine_translate_address(_db_status.prev_ip, FALSE, DEBUG_FETCH, _db_status.prev_mode);
    if (addr >= 0)
        memory_retrieve_raw_instr(prev_instr, addr);
    else
        prev_instr[0] = '\0';

    // Get the next instruction 
    addr = machine_translate_address(_db_status.ip, FALSE, DEBUG_FETCH, machine_get_mode());
    if (addr >= 0)
        memory_retrieve_raw_instr(next_instr, addr);
    else
        next_instr[0] = '\0';

    printf("Previous instruction at IP = %d: %s\n", _db_status.prev_ip, prev_instr);
    printf("Mode: %s \t PID: %d\n", (machine_get_mode() == PRIVILEGE_KERNEL) ? "KERNEL" : "USER", debug_active_process());
    printf("Next instruction at IP = %d, Page No. = %d: %s\n", _db_status.ip, _db_status.ip / XSM_PAGE_SIZE, next_instr);

    while (!done)
    {
        printf("debug> ");
        fgets(command, DEBUG_COMMAND_LEN, stdin);

        // Remove the dangling \n from fgets
        strtok(command, "\n");

        if (!strcmp(command, "\n"))
            strncpy(command, _db_status.command, DEBUG_COMMAND_LEN);
        else if (!strcmp(command, "exit") || !strcmp(command, "e"))
        {
            debug_deactivate();
            printf("Killing the machine\n");
            exit(0);
            return FALSE;
        }
        else
            strncpy(_db_status.command, command, DEBUG_COMMAND_LEN);

        done = debug_command(command);
    }

    return TRUE;
}

/* Call the function based on the given command */
int debug_command(char *command)
{
    int code;
    char *arg1, *cmd, *arg2;

    const char *delim = " \t";

    cmd = strtok(command, delim);
    code = debug_command_code(cmd);

    switch (code)
    {
    case DEBUG_STEP:
        arg1 = strtok(NULL, delim);
        if (arg1)
            debug_skip_n(atoi(arg1), DEBUG_STEP);

        return TRUE;

    case DEBUG_CONTINUE:
        arg1 = strtok(NULL, delim);
        if (arg1)
            debug_skip_n(atoi(arg1), DEBUG_CONTINUE);

        debug_deactivate();
        return TRUE;

    case DEBUG_REG:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_display_all_registers();
        else
        {
            arg2 = strtok(NULL, delim);
            if (!arg2)
                debug_display_register(arg1);
            else
                debug_display_range_reg(arg1, arg2);
        }
        break;

    case DEBUG_MEM:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_invalid_cmd(command);
        else
        {
            arg2 = strtok(NULL, delim);
            if (!arg2)
                debug_display_mem_range(atoi(arg1), atoi(arg1));
            else
                debug_display_mem_range(atoi(arg1), atoi(arg2));
        }
        break;

    case DEBUG_PCB:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_display_pcb();
        else
            debug_display_pcb_pid(atoi(arg1));
        break;

    case DEBUG_PAGETABLE:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_display_pt_ptbr();
        else
            debug_display_pt_pid(atoi(arg1));
        break;

    case DEBUG_FILETABLE:
        debug_display_ft();
        break;

    case DEBUG_SEMTABLE:
        debug_display_st();
        break;

    case DEBUG_FILESTATUS:
        debug_display_fst();
        break;

    case DEBUG_DISKSTATUS:
        debug_display_dst();
        break;

    case DEBUG_SYSTEMSTATUS:
        debug_display_sst();
        break;

    case DEBUG_TERMINALSTATUS:
        debug_display_tst();
        break;

    case DEBUG_BUFFERTABLE:
        debug_display_bt();
        break;

    case DEBUG_DISKMAPTABLE:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_display_dmt();
        else
            debug_dmt_pid(atoi(arg1));
        break;

    case DEBUG_ROOTFILE:
        debug_display_rf();
        break;

    case DEBUG_RESOURCETABLE:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_display_rt();
        else
            debug_rt_pid(atoi(arg1));
        break;

    case DEBUG_MEMFREELIST:
        debug_display_memlist();
        break;

    case DEBUG_DISKFREELIST:
        debug_display_dfl();
        break;

    case DEBUG_INODETABLE:
        debug_display_inodetable();
        break;

    case DEBUG_USERTABLE:
        debug_display_usertable();
        break;

    case DEBUG_LOCATION:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_invalid_cmd(command);
        else
            debug_display_location(atoi(arg1));
        break;

    case DEBUG_VAL:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_invalid_cmd(command);
        else
            debug_display_val(arg1);
        break;

    case DEBUG_WATCH:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_invalid_cmd(command);
        else
        {
            debug_watch_add(atoi(arg1));
            printf("Watch point added at %d.\n", atoi(arg1));
        }
        break;

    case DEBUG_WATCHCLEAR:
        debug_watch_clear();
        printf("Watch points cleared.\n");
        break;

    case DEBUG_LIST:
        debug_display_list();
        break;

    case DEBUG_PAGE:
        arg1 = strtok(NULL, delim);
        if (!arg1)
            debug_invalid_cmd(command);
        else
            debug_display_page(atoi(arg1));
        break;

    case DEBUG_HELP:
        debug_display_help();
        break;

    default:
        debug_invalid_cmd(command);
    }

    return FALSE;
}

void debug_invalid_cmd(const char *cmd)
{
    printf("Unknown command \"%s\". See \"help\" for more information.\n", cmd);
}

/* Retrieve the debug command index */
int debug_command_code(const char *cmd)
{
    int i;

    for (i = 0; i < DEBUG_COUNT; ++i)
        if (!strcmp(cmd, _db_commands_lh[i]))
            return i;

    for (i = 0; i < DEBUG_COUNT; ++i)
        if (!strcmp(cmd, _db_commands_sh[i]))
            return i;

    return DEBUG_ERROR;
}

/* Retrieve RUNNING process */
int debug_active_process()
{
    int ptbr, pid;

    ptbr = registers_get_integer("PTBR");
    pid = (ptbr - DEBUG_PT_BASE) / 20;

    if (pid < 0 || pid >= MAX_PROC_NUM)
        pid = -1;

    return pid;
}

/* Debug step/continue command */
int debug_skip_n(int num, int debug_command)
{
    num--;
    if (num > 0)
    {
        _db_status.skip = num;
        _db_status.skip_command = debug_command;
    }

    return TRUE;
}

/* Debug reg command */
int debug_display_all_registers()
{
    int num_regs, i;
    char *content;
    const char **reg_names;

    reg_names = registers_names();
    num_regs = registers_len();

    for (i = 0; i < num_regs; ++i)
    {
        content = registers_get_string(reg_names[i]);
        printf("%s: %s\t", reg_names[i], content);

        if ((i < 20 && i % 5 == 4) || i == 23 || i == 28)
            printf("\n");
    }

    printf("\n");
    return TRUE;
}

/* Debug reg regname command */
int debug_display_register(const char *regname)
{
    char *content = registers_get_string(regname);

    if (!content)
    {
        printf("No such register.\n");
        return FALSE;
    }

    printf("%s: %s\n", regname, content);
    return TRUE;
}

/* Debug reg reg_b_name reg_e_name command */
int debug_display_range_reg(const char *reg_b_name, const char *reg_e_name)
{
    int num_regs, i;
    char *content;
    const char **reg_names;

    reg_names = registers_names();
    num_regs = registers_len();

    for (i = 0; i < num_regs; ++i)
        if (!strcmp(reg_b_name, reg_names[i]))
            break;

    for (; i < num_regs; ++i)
    {
        content = registers_get_string(reg_names[i]);
        printf("%s: %s\n", reg_names[i], content);

        if (!strcmp(reg_e_name, reg_names[i]))
            break;
    }

    return TRUE;
}

/* Debug mem page_l page_r command */
int debug_display_mem_range(int page_l, int page_h)
{
    int i,j;
    int ptr;
    char *content;
    xsm_word *word;
    FILE *fp;
    fp = fopen("mem", "w");
    for (i = page_l; i <= page_h; ++i)
    {
        printf("Page: %d\n", i);
    	word = memory_get_page(i);
        if (!word)
    	{
        	printf("No such page.\n");
        	return FALSE;
    	}
	ptr = i * XSM_PAGE_SIZE;
        fprintf(fp, "Page: %d\n", i);
	// Write to file mem
    	for (j = 0; j < XSM_PAGE_SIZE; j++)
    	{
        	word = memory_get_word(ptr);
        	content = word_get_string(word);
        	fprintf(fp, "%d: %s\n", j, content);
        	ptr++;
    	}

    }
    printf("Written to file mem\n");
    fclose(fp);
    return TRUE;
}

int debug_display_fields(int baseptr, const char **fields, const int *fields_len, int n_fields)
{
    int i, ptr, num;
    xsm_word *word;

    const char *state[] = {"READY", "RUNNING", "CREATED", "TERMINATED", "WAIT_DISK", "WAIT_FILE", "WAIT_BUFFER", "WAIT_TERMINAL", "WAIT_PROCESS", "WAIT_SEMAPHORE", "WAIT_MEM", "ALLOCATED"};

    const char *mode[] = {"Create", "Open", "Close", "Delete", "Write", "Seek", "Read", "Fork", "Exec", "Exit", "Getpid", "Getppid", "Wait", "Signal", "15", "16", "Semget", "Semrelease", "SemLock", "SemUnLock", "Shutdown", "Newusr", "Remusr", "Setpwd", "Getuname", "Getuid", "Login", "Logout"};

    const char *test[] = {"Test0", "Test1", "Test2", "Test3"};

    ptr = baseptr;

    for (i = 0; i < n_fields; ++i)
    {
        printf("%s: \t\t", fields[i]);

        /* Convert STATE to CONSTANT */
        if (!strcmp(fields[i], "State"))
        {
            word = memory_get_word(ptr);
            num = word_get_integer(word);
            if (num >= 1 && num <= 12)
                printf("(%s, ", state[num - 1]);
            else
                printf("(%s, ", word_get_string(word));
            ptr = ptr + 1;

            word = memory_get_word(ptr);
            printf("%s)\n", word_get_string(word));
            ptr = ptr + 1;

            continue;
        }

        /* Convert MODE to CONSTANT */
        if (!strcmp(fields[i], "Mode Flag"))
        {
            word = memory_get_word(ptr);
            num = word_get_integer(word);
            if (num >= 1 && num <= 28)
                printf("%s", mode[num - 1]);
            else if (num >= 96 && num <= 99)
                printf("%s", test[num - 96]);
            else
                printf("%s", word_get_string(word));
            ptr = ptr + 1;

            printf("\n");

            continue;
        }

        /* Display the corresponding number of words. */
        int j;
        for (j = 0; j < fields_len[i]; ++j)
        {
            word = memory_get_word(ptr);
            printf("%s ", word_get_string(word));
            ptr = ptr + 1;
        }

        printf("\n");
    }

    return TRUE;
}

/* Debug pcb pid command */
int debug_display_pcb_pid(int pid)
{
    const char *fields[] = {"Tick", "PID", "PPID", "UserID", "State", "Swap Flag", "Inode Index",
                            "Input Buffer", "Mode Flag", "User Area Swap Status", "User Area Page Number",
                            "Kernel Stack Pointer", "User Stack Pointer", "PTBR", "PTLR", "Unused"};

    const int fields_len[] = {1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    const int n_fields = 15;
    int ptr;

    ptr = DEBUG_LOC_PT + pid * PT_ENTRY_SIZE;

    return debug_display_fields(ptr, fields, fields_len, n_fields);
}

/* Debug pcb command */
int debug_display_pcb()
{
    int pid = debug_active_process();

    if (pid > -1)
    {
        debug_display_pcb_pid(pid);
        return TRUE;
    }

    printf("No active processes.\n");
    return FALSE;
}

/* Debug pagetable command */
int debug_display_pt_ptbr()
{
    int addr = registers_get_integer("PTBR");
    return debug_display_pt_at(addr);
}

/* Display Page Table at given addr */
int debug_display_pt_at(int addr)
{
    int i, ptr;
    xsm_word *word;

    ptr = addr;

    for (i = 0; i < MAX_NUM_PAGES; i++)
    {
        printf("VIRT: %d\t\t", i);

        word = memory_get_word(ptr);
        printf("PHY: %s\t\t", word_get_string(word));
        ptr = ptr + 1;

        word = memory_get_word(ptr);
        printf("AUX: %s\t\n", word_get_string(word));
        ptr = ptr + 1;
    }

    return TRUE;
}

/* Debug pagetable pid command */
int debug_display_pt_pid(int pid)
{
    int ptbr_addr = DEBUG_PT_BASE + pid * MAX_NUM_PAGES * 2;
    return debug_display_pt_at(ptbr_addr);
}

/* Debug diskmaptable command */
int debug_display_dmt()
{
    int pid = debug_active_process();

    if (pid > -1)
        return debug_dmt_pid(pid);

    printf("No active processes.\n");
    return FALSE;
}

/* Debug diskmaptable pid command */
int debug_dmt_pid(int pid)
{
    int ptr;
    xsm_word *word;

    ptr = DEBUG_LOC_DISKMAPTABLE + pid * MAX_NUM_PAGES + 2;

    word = memory_get_word(ptr++);
    printf("Heap 1 in Disk: %s\t", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Heap 2 in Disk: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Code 1 in Disk: %s\t", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Code 2 in Disk: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Code 3 in Disk: %s\t", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Code 4 in Disk: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Stack 1 in Disk: %s\t", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Stack 2 in Disk: %s\n", word_get_string(word));

    return TRUE;
}

/* Debug resourcetable command */
int debug_display_rt()
{
    int pid = debug_active_process();

    if (pid > -1)
        return debug_rt_pid(pid);

    printf("No active processes.\n");
    return FALSE;
}

/* Debug resourcetable pid command */
int debug_rt_pid(int pid)
{
    int ptr, page, rid, i;
    xsm_word *word;

    page = DEBUG_LOC_PT + pid * PT_ENTRY_SIZE + 11;
    word = memory_get_word(page);
    page = word_get_integer(word);

    if (page < 0 || page >= XSM_MEMORY_NUMPAGES)
    {
        printf("Invalid User Area Page Number");
        return FALSE;
    }

    ptr = page * XSM_PAGE_SIZE + RESOURCE_OFFSET;

    for (i = 0; i < MAX_RESOURCE; ++i)
    {
        printf("%d. ", i);

        word = memory_get_word(ptr++);
        rid = word_get_integer(word);
        printf("Resource Identifier: ");

        switch (rid)
        {
        case 0:
            printf("FILE\t\t");
            break;
        case 1:
            printf("SEMAPHORE\t");
            break;
        default:
            printf("%s\t\t", word_get_string(word));
            break;
        }

        word = memory_get_word(ptr++);
        printf("Index of Table Entry: %s\n", word_get_string(word));
    }

    return TRUE;
}

/* Debug filetable command */
int debug_display_ft()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_FILETABLE;

    for (i = 0; i < MAX_OPENFILE_NUM; ++i)
    {
        printf("%d. ", i);

        word = memory_get_word(ptr++);
        printf("Inode Index: %s\t", word_get_string(word));

        word = memory_get_word(ptr++);
        printf("Open Instance Count: %s\t", word_get_string(word));

        word = memory_get_word(ptr++);
        printf("Lseek: %s\n", word_get_string(word));

        ptr++; /* Unused field. */
    }

    return TRUE;
}

/* Debug semtable command */
int debug_display_st()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_SEMTABLE;

    for (i = 0; i < MAX_SEM_COUNT; ++i)
    {
        printf("%d. ", i);

        word = memory_get_word(ptr++);
        printf("Locking PID: %s\t", word_get_string(word));

        word = memory_get_word(ptr++);
        printf("Process Count: %s\n", word_get_string(word));

        ptr += 2; /* Unused field. */
    }

    return TRUE;
}

/* Debug memfreelist command */
int debug_display_memlist()
{
    int i, ptr;
    xsm_word *word;

    ptr = DEBUG_LOC_MFL;

    for (i = 0; i < MAX_MEM_PAGE;)
    {
        word = memory_get_word(ptr++);
        printf("%d\t%s\t\t", i, word_get_string(word));

        word = memory_get_word(ptr++);
        printf("%d\t%s\t\t", i + 1, word_get_string(word));

        word = memory_get_word(ptr++);
        printf("%d\t%s\t\t", i + 2, word_get_string(word));

        word = memory_get_word(ptr++);
        printf("%d\t%s\n", i + 3, word_get_string(word));

        i = i + 4;
    }

    return TRUE;
}

/* Debug filestatus command */
int debug_display_fst()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_FILESTATUS;

    for (i = 0; i < MAX_FILE_NUM; ++i)
    {
        printf("%d. ", i);

        word = memory_get_word(ptr++);
        printf("Locking PID: %s\t", word_get_string(word));

        word = memory_get_word(ptr++);
        printf("File Open Count: %s\n", word_get_string(word));

        ptr += 2; /* Unused field. */
    }

    return TRUE;
}

/* Debug diskstatus command */
int debug_display_dst()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_DISKSTATUS;

    word = memory_get_word(ptr++);
    printf("Status: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Load/Store Bit: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Page Number: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Block Number: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("PID: %s\n", word_get_string(word));

    ptr += 3; /* Unused field. */

    return TRUE;
}

/* Debug systemstatus command */
int debug_display_sst()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_SYSTEMSTATUS;

    word = memory_get_word(ptr++);
    printf("Current User ID: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Current PID: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Memory Free Count: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Wait Memory Count: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Swapped Count: %s\n", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("Paging Status: %s\n", word_get_string(word));

    ptr += 2; /* Unused field. */

    return TRUE;
}

/* Debug terminalstatus command */
int debug_display_tst()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_TERMINALSTATUS;

    word = memory_get_word(ptr++);
    printf("Status: %s\t", word_get_string(word));

    word = memory_get_word(ptr++);
    printf("PID: %s\n", word_get_string(word));

    ptr += 2; /* Unused field. */

    return TRUE;
}

/* Debug buffertable command */
int debug_display_bt()
{
    int ptr, i;
    xsm_word *word;

    ptr = DEBUG_LOC_BUFFERTABLE;

    for (i = 0; i < MAX_BUFFER; ++i)
    {
        printf("%d. ", i);

        word = memory_get_word(ptr++);
        printf("Block Number: %s\t", word_get_string(word));

        word = memory_get_word(ptr++);
        printf("Dirty Bit: %s\t", word_get_string(word));

        word = memory_get_word(ptr++);
        printf("Locking PID: %s\n", word_get_string(word));

        ptr += 1; /* Unused field. */
    }

    return TRUE;
}

/* Debug inodetable command */
int debug_display_inodetable()
{
    int i, ptr;
    const char *fields[] = {
        "Type", "Name", "Size", "UID", "Perm.", "Unused", "D1", "D2", "D3", "D4", "Unused"};
    const int fields_len[] = {
        1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 4};
    const int n_fields = 11;
    const int entry_size = 16;

    ptr = DEBUG_LOC_INODE;

    for (i = 0; i < MAX_FILE_NUM; i++)
    {
        debug_display_fields(ptr, fields, fields_len, n_fields);
        printf("\n");

        /* Size of each entry is 16 */
        ptr = ptr + entry_size;
    }

    return TRUE;
}

/* Debug usertable command */
int debug_display_usertable()
{
    const char *fields[] = {
        "User name", "Encrypted Password"};
    const int fields_len[] = {
        1, 1};
    int i, ptr;
    const int n_fields = 2;
    const int entry_size = 2;

    ptr = DEBUG_LOC_USERTABLE;

    for (i = 0; i < MAX_USER_NUM; i++)
    {
        debug_display_fields(ptr, fields, fields_len, n_fields);
        printf("\n");

        /* Update ptr */
        ptr = ptr + entry_size;
    }

    return TRUE;
}

/* Debug diskfreelist command */
int debug_display_dfl()
{
    int i, ptr;
    xsm_word *word;

    ptr = DEBUG_LOC_DFL;

    for (i = 0; i < XSM_DISK_BLOCK_SIZE; i += 4)
    {
        word = memory_get_word(ptr++);
        printf("%d\t%s\t\t", i, word_get_string(word));

        word = memory_get_word(ptr++);
        printf("%d\t%s\t\t", i + 1, word_get_string(word));

        word = memory_get_word(ptr++);
        printf("%d\t%s\t\t", i + 2, word_get_string(word));

        word = memory_get_word(ptr++);
        printf("%d\t%s\n", i + 3, word_get_string(word));
    }

    return TRUE;
}

int debug_display_rf()
{
    const char *fields[] = {
        "File Name",
        "File Size",
        "File Type",
        "User Name",
        "Permission",
    };
    const int fields_len[] = {
        1,
        1,
        1,
        1,
        1,
    };

    int i, ptr;
    const int n_fields = 5;
    const int entry_size = 8;

    ptr = DEBUG_LOC_ROOTFILE;

    for (i = 0; i < MAX_FILE_NUM; ++i)
    {
        debug_display_fields(ptr, fields, fields_len, n_fields);
        printf("\n");

        /* Size of each entry is 16 */
        ptr = ptr + entry_size;
    }

    return TRUE;
}

/* Display location command */
int debug_display_location(int loc)
{
    int mode, ptbr, ptlr, tr_loc;
    xsm_word *word;

    mode = machine_get_mode();

    if (mode == PRIVILEGE_KERNEL)
        word = memory_get_word(loc);
    else
    {
        ptbr = registers_get_integer("PTBR");
        ptlr = registers_get_integer("PTLR");
        tr_loc = memory_translate_address(ptbr, ptlr, loc, FALSE);

        if (tr_loc < 0)
        {
            printf("A translation for this address is not available.\n");
            return FALSE;
        }

        word = memory_get_word(tr_loc);
    }

    printf("%s\n", word_get_string(word));
    return TRUE;
}

/* debug val command */
int debug_display_val(char *mem)
{
    xsm_word *mword = memory_get_word(atoi(mem));
    printf("%s\n", word_get_string(mword));

    return TRUE;
}

/* Debug watch command */
int debug_watch_add(int loc)
{
    if (_db_status.wp_size >= DEBUG_MAX_WP)
        return FALSE;

    _db_status.wp[_db_status.wp_size] = loc;
    _db_status.wp_size++;

    return TRUE;
}

/* Debug watchclear command */
void debug_watch_clear()
{
    _db_status.wp_size = 0;
}

/* Check if any watch points have been triggered */
int debug_watch_test(int mem_left, int mem_right)
{
    int i;

    if (mem_left < 0)
        return DEBUG_ERROR;

    for (i = 0; i < _db_status.wp_size; ++i)
        if (mem_left == _db_status.wp[i] || _db_status.wp[i] == mem_right)
            return i;

    return DEBUG_ERROR;
}

/* Debug list command */
int debug_display_list()
{
    int i, addr;
    char instr[DEBUG_STRING_LEN];

    for (i = 0; i <= 2 * DEBUG_LIST_LEN; i++)
    {
        addr = machine_translate_address(_db_status.ip + (i - DEBUG_LIST_LEN) * XSM_INSTRUCTION_SIZE, FALSE, DEBUG_FETCH, machine_get_mode());
        if (addr >= 0)
            memory_retrieve_raw_instr(instr, addr);
        else
            instr[0] = '\0';
        if (i == DEBUG_LIST_LEN)
            printf("%d* \t %s \n", _db_status.ip + (i - DEBUG_LIST_LEN) * XSM_INSTRUCTION_SIZE, instr);
        else
            printf("%d \t %s \n", _db_status.ip + (i - DEBUG_LIST_LEN) * XSM_INSTRUCTION_SIZE, instr);
    }

    return TRUE;
}

/* Debug page command */
int debug_display_page(int ip)
{
    if (ip < 0 || ip >= XSM_MEMORY_NUMPAGES * XSM_PAGE_SIZE)
    {
        printf("Invalid IP\n");
        return FALSE;
    }

    printf("Page Number: %d \t Offset: %d\n", ip / XSM_PAGE_SIZE, ip % XSM_PAGE_SIZE);
    return TRUE;
}

/* Debug help command */
void debug_display_help()
{
    printf(" step / s \n\t Execution proceeds by a single step \n");
    printf(" step / s <N> \n\t Execution proceeds by N number of steps \n");
    printf(" continue / c \n\t Execution proceeds till the next BRKP instruction \n");
    printf(" continue / c <N> \n\t Execution proceeds till the next N'th occurence of the BRKP instruction \n");
    printf(" reg / r \n\t Displays the contents of all the machine registers \n");
    printf(" reg / r <register_name>  \n\t Displays the contents of the specified register \n");
    printf(" mem / m <page_num>  \n\t Writes the contents of the memory page <page_num> to the file \"mem\" \n");
    printf(" mem / m <page_num_1> <page_num_2>  \n\t Writes the contents of the memory from pages <page_num_1> to <page_num_2> to the file \"mem\" \n");
    printf(" pcb / p \n\t Displays the Process Table entry of the current process \n");
    printf(" pcb / p <pid> \n\t Displays the Process Table entry of the process with the given <pid> \n");
    printf(" pagetable / pt \n\t Displays the Page Table at the location pointed by PTBR \n");
    printf(" pagetable / pt <pid> \n\t Displays the <pid> th Page Table \n");
    printf(" diskmaptable / dmt \n\t Displays the Disk Map Table of the current process \n");
    printf(" diskmaptable / dmt <pid> \n\t Displays the Disk Map Table of the process with the given <pid> \n");
    printf(" resourcetable / rt \n\t Displays the Per-process Resource Table of the current process \n");
    printf(" resourcetable / rt <pid> \n\t Displays the Per-process Resource Table of the process with the given <pid> \n");
    printf(" filetable / ft \n\t Displays the Open File Table \n");
    printf(" semtable / st \n\t Displays the Semaphore Table \n");
    printf(" memfreelist / mf \n\t Displays the Memory Free List \n");
    printf(" filestatus / fst \n\t Displays the File Status Table \n");
    printf(" diskstatus / dst \n\t Displays the Disk Status Table \n");
    printf(" systemstatus / sst \n\t Displays the System Status Table \n");
    printf(" terminalstatus / tst \n\t Displays the Terminal Status Table \n");
    printf(" buffertable / bt \n\t Displays the Buffer Table \n");
    printf(" inodetable / it \n\t Displays the memory copy of the Inode Table \n");
    printf(" usertable / ut \n\t Displays the memory copy of the User Table \n");
    printf(" diskfreelist / df \n\t Displays the memory copy of Disk Free List \n");
    printf(" rootfile / rf \n\t Displays the memory copy of the Root File \n");
    printf(" location / loc <address> \n\t Displays the content at memory address (address translation takes place if used in USER mode) \n");
    printf(" val / v <address> \n\t Displays the content at memory address (no address translation occurs) \n");
    printf(" watch / w <physical_address> \n\t Sets a watch point at this address \n");
    printf(" watchclear / wc \n\t Clears all the watch points \n");
    printf(" list / l \n\t List 10 instructions before and after the current instruction \n");
    printf(" page / pg <address> \n\t Displays the Page Number and Offset for the given <address> \n");
    printf(" exit / e \n\t Exits the debug prompt and halts the machine \n");
}
