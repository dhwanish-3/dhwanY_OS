/*
The XFS disk.
*/

#include "disk.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static xsm_word *_disk_mem_copy;

static FILE *_file;

static int _mem_size;

/* Initialise disk */
int disk_init(const char *filename)
{
    _mem_size = sizeof(xsm_word) * XSM_DISK_BLOCK_SIZE * XSM_DISK_BLOCK_NUM;

    /* Acquire memory for saving the memory copy. */
    _disk_mem_copy = (xsm_word *)malloc(_mem_size);

    if (!_disk_mem_copy)
        return XSM_FAILURE;

    memset(_disk_mem_copy, 0, _mem_size);
    _file = fopen(filename, "rb+");

    if (_file)
        fread(_disk_mem_copy, 1, _mem_size, _file);
    else
    {
        /* If the file does not exist, create one */
        _file = fopen(filename, "wb+");
        if (!_file)
            return XSM_FAILURE;
    }

    return XSM_SUCCESS;
}

/* Writes page to the given block */
int disk_write_page(xsm_word *page, int block_num)
{
    xsm_word *block = disk_get_block(block_num);
    memcpy(block, page, XSM_PAGE_SIZE * XSM_WORD_SIZE);
    return TRUE;
}

/* Retrieve the block for the given block number */
xsm_word *disk_get_block(int block)
{
    char *addr, *block_base;
    size_t offset;

    addr = (char *)_disk_mem_copy;
    offset = block * XSM_DISK_BLOCK_SIZE * XSM_WORD_SIZE;
    block_base = addr + offset;

    return (xsm_word *)block_base;
}

/* Writes from block to the given page */
int disk_read_block(xsm_word *page, int block_num)
{
    xsm_word *block = disk_get_block(block_num);
    memcpy(page, block, XSM_PAGE_SIZE * XSM_WORD_SIZE);
    return TRUE;
}

/* Deallocate the disk */
int disk_close()
{
    int result;

    /* Clean the disk */
    fclose(_file);

    /* Commit changes to disk */
    _file = fopen(XSM_DEFAULT_DISK, "w");
    result = fwrite(_disk_mem_copy, 1, _mem_size, _file);
    fclose(_file);

    return result;
}