#ifndef XFS_INODE_H

#define XFS_INODE_H

#include "disk.h"
#include "virtualDisk.h"

int FindEmptyInodeEntry();
int getInodeEntry(char *name);
void AddEntryToMemInode(int startIndex, int fileType, char *nameOfFile, int fileSize, int *addrOfDataBlocks);
void AddEntryToMemRootFile(int startIndexInRootFile, int fileType, char *nameOfFile, int size_of_file);
int removeInodeEntry(int locationOfInode);
int removeRootFileEntry(int locationOfRootFile);
int getDataBlocks(int *dataBlockAddr, int locationOfInode);

#endif