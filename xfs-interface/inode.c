/*
Interface between the XFS files and disk.
*/

#include "inode.h"

#include <stdio.h>
#include <string.h>

extern BLOCK *disk;

/* Returns an empty FAT entry */
int FindEmptyInodeEntry()
{
    int i, j, entryFound, entryNumber;

    entryFound = 0;
    entryNumber = 0;

    for (j = INODE; j < INODE + NO_OF_INODE_BLOCKS; j++)
    {
        for (i = INODE_ENTRY_FILENAME; i < BLOCK_SIZE; i += INODE_ENTRY_SIZE)
        {
            if (getValue(disk[j].word[i]) == XFS_FAILURE)
            {
                entryNumber = ((j - INODE) * BLOCK_SIZE) + i;
                entryFound = TRUE;
                break;
            }
        }

        if (entryFound == TRUE)
            break;
    }

    if (entryNumber > INODE_SIZE)
    {
        printf("INODE is full\n");
        return XFS_FAILURE;
    }

    return (entryNumber - INODE_ENTRY_FILENAME);
}

/* Checks if a file having the given name is present on the disk */
int getInodeEntry(char *name)
{
    int i, j;

    if (name == NULL)
        return -1;

    for (j = INODE; j < INODE + NO_OF_INODE_BLOCKS; j++)
    {
        for (i = INODE_ENTRY_FILENAME; i < BLOCK_SIZE; i = i + INODE_ENTRY_SIZE)
        {
            if (j == (INODE + NO_OF_INODE_BLOCKS - 1) && i >= (960 - BLOCK_SIZE))
                continue;

            if (strcmp(disk[j].word[i], name) == 0 && getValue(disk[j].word[i]) != XFS_FAILURE)
                return (((j - INODE) * BLOCK_SIZE) + i - INODE_ENTRY_FILENAME);
        }
    }

    return XFS_FAILURE;
}

/* Adds the name, size and data block addresses of the file to corresponding entry in the inode */
void AddEntryToMemInode(int startIndex, int fileType, char *nameOfFile, int fileSize, int *addrOfDataBlocks)
{
    int i, baseAddress;

    baseAddress = INODE * BLOCK_SIZE + startIndex;

    storeValueAt(baseAddress + INODE_ENTRY_FILETYPE, fileType);
    storeStringValueAt(baseAddress + INODE_ENTRY_FILENAME, nameOfFile);
    storeValueAt(baseAddress + INODE_ENTRY_FILESIZE, fileSize);

    if (fileType == FILETYPE_ROOT)
    {
        storeValueAt(baseAddress + INODE_ENTRY_USERID, 0);
        storeValueAt(baseAddress + INODE_ENTRY_PERMISSION, 0);
    }
    else if (fileType == FILETYPE_DATA)
    {
        storeValueAt(baseAddress + INODE_ENTRY_USERID, 1);
        storeValueAt(baseAddress + INODE_ENTRY_PERMISSION, 1);
    }
    else if (fileType == FILETYPE_EXEC)
    {
        storeValueAt(baseAddress + INODE_ENTRY_USERID, 0);
        storeValueAt(baseAddress + INODE_ENTRY_PERMISSION, -1);
    }

    for (i = 0; i < INODE_NUM_DATA_BLOCKS; i++)
        storeValueAt(baseAddress + INODE_ENTRY_DATABLOCK + i, addrOfDataBlocks[i]);

    AddEntryToMemRootFile(startIndex / INODE_ENTRY_SIZE * ROOTFILE_ENTRY_SIZE, fileType, nameOfFile, fileSize);
}

/* Adds the type, name and size of the file to corresponding entry in the rootfile */
void AddEntryToMemRootFile(int startIndexInRootFile, int fileType, char *nameOfFile, int size_of_file)
{
    int baseAddress = ROOTFILE * BLOCK_SIZE + startIndexInRootFile;

    storeStringValueAt(baseAddress + ROOTFILE_ENTRY_FILENAME, nameOfFile);
    storeValueAt(baseAddress + ROOTFILE_ENTRY_FILESIZE, size_of_file);
    storeValueAt(baseAddress + ROOTFILE_ENTRY_FILETYPE, fileType);
}

/* Removes the Inode entry corresponding to the first arguement */
int removeInodeEntry(int locationOfInode)
{
    int i, blockNumber, startWordNumber;

    blockNumber = INODE + locationOfInode / BLOCK_SIZE;
    startWordNumber = locationOfInode % BLOCK_SIZE;

    storeValue(disk[blockNumber].word[startWordNumber + INODE_ENTRY_FILETYPE], -1);
    storeValue(disk[blockNumber].word[startWordNumber + INODE_ENTRY_FILENAME], -1);
    storeValue(disk[blockNumber].word[startWordNumber + INODE_ENTRY_FILESIZE], 0);
    storeValue(disk[blockNumber].word[startWordNumber + INODE_ENTRY_USERID], -1);
    storeValue(disk[blockNumber].word[startWordNumber + INODE_ENTRY_PERMISSION], -1);

    for (i = 0; i < INODE_NUM_DATA_BLOCKS; i++)
        storeValue(disk[blockNumber].word[startWordNumber + INODE_ENTRY_DATABLOCK + i], -1);

    removeRootFileEntry(locationOfInode / INODE_ENTRY_SIZE * ROOTFILE_ENTRY_SIZE);

    return XFS_SUCCESS;
}

/* Removes the Root file entry corresponding to the first argument */
int removeRootFileEntry(int locationOfRootFile){
    int i, blockNumber, startWordNumber;

	blockNumber = ROOTFILE + locationOfRootFile / BLOCK_SIZE;
	startWordNumber = locationOfRootFile % BLOCK_SIZE;

	storeValue(disk[blockNumber].word[startWordNumber + ROOTFILE_ENTRY_FILETYPE], -1);
	storeValue(disk[blockNumber].word[startWordNumber + ROOTFILE_ENTRY_FILENAME], -1);
	storeValue(disk[blockNumber].word[startWordNumber + ROOTFILE_ENTRY_FILESIZE], 0);
	
	return XFS_SUCCESS;
}

/* Returns the data block entries corresponding to the address specified by the locationOfInode */
int getDataBlocks(int *dataBlockAddr, int locationOfInode)
{
    int i, diskNum, wordNum;

    diskNum = INODE + locationOfInode / BLOCK_SIZE;
    wordNum = INODE_ENTRY_DATABLOCK + locationOfInode % BLOCK_SIZE;

    for (i = 0; i < INODE_NUM_DATA_BLOCKS; i++)
        dataBlockAddr[i] = getValue(disk[diskNum].word[wordNum + i]);

    return XFS_SUCCESS;
}