/*
An interface that handles XSM memory.
*/

#include "memory.h"

#include <stdlib.h>
#include <string.h>

static xsm_word *_xsm_mem;

/* Initialse the RAM */
int memory_init()
{
    _xsm_mem = (xsm_word *)malloc(sizeof(xsm_word) * XSM_MEMORY_SIZE);

    if (!_xsm_mem)
        return XSM_FAILURE;

    return XSM_SUCCESS;
}

/* Returns the word stored in the given address */
xsm_word *memory_get_word(int address)
{
    if (!memory_is_address_valid(address))
        return NULL;

    return &_xsm_mem[address];
}

/* Checks whether the given address is valid */
int memory_is_address_valid(int address)
{
    if (address >= XSM_MEMORY_SIZE || address < 0)
        return FALSE;

    return TRUE;
}

/* Returns the page number for the given address */
int memory_addr_page(int address)
{
    int page;

    if (address < 0)
        return -1;

    page = address / XSM_PAGE_SIZE;
    return page;
}

/* Converts the virtual address to physical address */
int memory_translate_address(int ptbr, int ptlr, int address, int write)
{
    int page, offset;
    int target_page;

    page = memory_addr_page(address);
    offset = address % XSM_PAGE_SIZE;

    target_page = memory_translate_page(ptbr, ptlr, page, write);

    if (target_page < 0)
        return target_page;

    return target_page * XSM_PAGE_SIZE + offset;
}

/* Returns the physical page for the given virtual address */
int memory_translate_page(int ptbr, int ptlr, int page, int write)
{
    int page_entry, page_info, entry;
    xsm_word *page_entry_w, *page_info_w;
    char *info;

    if (page < 0 || page >= ptlr)
        return XSM_MEM_ILLPAGE;

    page_entry = page * 2 + ptbr;
    page_info = page_entry + 1;

    page_entry_w = memory_get_word(page_entry);
    page_info_w = memory_get_word(page_info);

    entry = word_get_integer(page_entry_w);
    info = word_get_string(page_info_w);

    if (info[1] == '0')
        return XSM_MEM_PAGEFAULT;

    if (write && info[2] == '0')
        return XSM_MEM_NOWRITE;

    return entry;
}

/* Returns the instruction at the gievn address */
void memory_retrieve_raw_instr(char *dest, int address)
{
    int i;
    xsm_word *instr = memory_get_word(address++);

    strcpy(dest, word_get_string(instr));

    for (i = 1; i < XSM_INSTRUCTION_SIZE; ++i)
    {
        instr = memory_get_word(address++);
        strcat(dest, word_get_string(instr));
    }
}

/* Converts page number to physical address */
xsm_word *memory_get_page(int page)
{
    return memory_get_word(page * XSM_PAGE_SIZE);
}

/* Deallocates the RAM */
void memory_destroy()
{
    free(_xsm_mem);
}