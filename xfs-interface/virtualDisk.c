/*
Interface between disk and memory copy of disk file.
*/

#include "virtualDisk.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern BLOCK *disk;

/* Initialise virtual disk */
void virtual_disk_init()
{
    disk = (BLOCK *)malloc(XFS_NUM_BLOCKS * sizeof(BLOCK));
}

/* Empties a block in the memory copy of the disk file */
void emptyBlock(int blockNo)
{
    int i;

    for (i = 0; i < BLOCK_SIZE; i++)
        strcpy(disk[blockNo].word[i], "");
}

/* Frees the blocks specified by the block number */
void freeUnusedBlock(int *freeBlock, int size)
{
    int i = 0;

    for (i = 0; i < size && freeBlock[i] != -1 && freeBlock[i] != 0; i++)
    {
        storeValueAt(DISK_FREE_LIST * BLOCK_SIZE + freeBlock[i], 0);
        emptyBlock(TEMP_BLOCK);
        writeToDisk(TEMP_BLOCK, freeBlock[i]);
    }
}

/* Returns the address of a free block on the disk */
int FindFreeBlock()
{
    int i, j;

    for (i = DISK_FREE_LIST; i < DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)
    {
        for (j = 0; j < BLOCK_SIZE; j++)
        {
            if (getValue(disk[i].word[j]) == 0)
            {
                storeValue(disk[i].word[j], 1);
                return ((i - DISK_FREE_LIST) * BLOCK_SIZE + j);
            }
        }
    }

    return XFS_FAILURE;
}

/* Fills the memory copy of disk data structures with default values */
void setDefaultValues(int dataStructure)
{
    int i, j;

    switch (dataStructure)
    {
    case DISK_FREE_LIST:
        for (j = 0; j < (NO_OF_FREE_LIST_BLOCKS * BLOCK_SIZE); j++)
        {
            i = j / BLOCK_SIZE;
            if ((j >= DATA_START_BLOCK) && (j < NO_OF_DISK_BLOCKS))
                storeValue(disk[DISK_FREE_LIST + i].word[j], 0);
            else
                storeValue(disk[DISK_FREE_LIST + i].word[j], 1);
        }
        break;

    case INODE:
        for (i = 0; i < BLOCK_SIZE; i++)
            storeValue(disk[INODE].word[i], -1);

        for (i = 0; i < BLOCK_SIZE; i += INODE_ENTRY_SIZE)
            storeValue(disk[INODE].word[INODE_ENTRY_FILESIZE + i], 0);

        for (i = 0; i < BLOCK_SIZE; i += INODE_ENTRY_SIZE)
            storeValue(disk[INODE].word[INODE_ENTRY_FILENAME + i], -1);

        for (i = 0; i < 960 - BLOCK_SIZE; i++)
            storeValue(disk[INODE + 1].word[i], -1);

        for (i = 0; i < 960 - BLOCK_SIZE; i += INODE_ENTRY_SIZE)
            storeValue(disk[INODE + 1].word[INODE_ENTRY_FILESIZE + i], 0);

        for (i = 0; i < 960 - BLOCK_SIZE; i += INODE_ENTRY_SIZE)
            storeValue(disk[INODE + 1].word[INODE_ENTRY_FILENAME + i], -1);

        for (i = 960 - BLOCK_SIZE; i < BLOCK_SIZE; i++)
            storeValue(disk[INODE + 1].word[i], -1);
        break;

    case ROOTFILE:
        for (j = 0; j < NO_OF_ROOTFILE_BLOCKS; j++)
        {
            for (i = 0; i < BLOCK_SIZE; i++)
                storeValue(disk[ROOTFILE + j].word[i], -1);

            for (i = 0; i < BLOCK_SIZE; i += ROOTFILE_ENTRY_SIZE)
                storeValue(disk[ROOTFILE + j].word[ROOTFILE_ENTRY_FILESIZE + i], 0);

            for (i = 0; i < BLOCK_SIZE; i += ROOTFILE_ENTRY_SIZE)
                storeValue(disk[ROOTFILE + j].word[ROOTFILE_ENTRY_FILENAME + i], -1);
        }
        break;
    }
}

/* Commits the memory copy of the disk data structures specified to the disk */
void commitMemCopyToDisk(int dataStructure)
{
    int i;

    switch (dataStructure)
    {
    case DISK_FREE_LIST:
        for (i = 0; i < NO_OF_FREE_LIST_BLOCKS; i++)
            writeToDisk(DISK_FREE_LIST + i, DISK_FREE_LIST + i);
        break;

    case INODE: //Also commit Root File
        for (i = 0; i < NO_OF_INODE_BLOCKS; i++)
            writeToDisk(INODE + i, INODE + i);

    case ROOTFILE:
        for (i = 0; i < NO_OF_ROOTFILE_BLOCKS; i++)
            writeToDisk(ROOTFILE + i, ROOTFILE + i);
        break;
    }
}

/* Returns all the XFS files */
XOSFILE *_getAllFiles()
{
    diskCheckFileExists();

    int i, j;
    XOSFILE *sentinel, *curr_ptr;
    int hasFiles = 0; // Flag which indicates if disk has no files

    // The sentinel works as a sentinel
    sentinel = malloc(sizeof(XOSFILE));
    sentinel->next = NULL;
    curr_ptr = sentinel;

    for (j = INODE; j < INODE + NO_OF_INODE_BLOCKS; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i = i + INODE_ENTRY_SIZE)
        {
            if (getValue(disk[j].word[INODE_ENTRY_FILENAME + i]) != -1)
            {
                if (j == INODE + NO_OF_INODE_BLOCKS - 1 && i >= 960 - BLOCK_SIZE)
                    continue;

                hasFiles = 1;
                XOSFILE *new_entry;

                new_entry = malloc(sizeof(XOSFILE));
                new_entry->name = strdup(disk[j].word[i + INODE_ENTRY_FILENAME]);
                new_entry->size = getValue(disk[j].word[i + INODE_ENTRY_FILESIZE]);

                curr_ptr->next = new_entry;
                curr_ptr = new_entry;
                curr_ptr->next = NULL;
            }
        }
    }

    curr_ptr = sentinel->next;
    free(sentinel);
    return curr_ptr;
}

/* Initialises the memory copy of the disk with the contents from the actual disk */
int loadFileToVirtualDisk()
{
    int i;

    for (i = DISK_FREE_LIST; i < DISK_FREE_LIST + NO_OF_FREE_LIST_BLOCKS; i++)
        readFromDisk(i, i);

    for (i = INODE; i < INODE + NO_OF_INODE_BLOCKS; i++)
        readFromDisk(i, i);

    for (i = ROOTFILE; i < ROOTFILE + NO_OF_ROOTFILE_BLOCKS; i++)
        readFromDisk(i, i);

    return XFS_SUCCESS;
}

/* Wipes out the entire contents of the memory copy of the disk */
void clearVirtDisk()
{
    bzero(disk, sizeof(BLOCK));
}

/* Retrieves a word from memory copy of the disk */
int getValueAt(int address)
{
    return getValue(disk[(address / BLOCK_SIZE)].word[(address % BLOCK_SIZE)]);
}

/* char* to int conversion */
int getValue(char *str)
{
    return atoi(str);
}

/* Retrieves a word from memory copy of the disk */
void storeValueAt(int address, int num)
{
    storeValue(disk[(address / BLOCK_SIZE)].word[(address % BLOCK_SIZE)], num);
}

/* char* to int conversion */
void storeValue(char *str, int num)
{
    sprintf(str, "%d", num);
}

/* Stores value at address */
void storeStringValueAt(int address, char *value)
{
    strcpy(disk[(address / BLOCK_SIZE)].word[(address % BLOCK_SIZE)], value);
}