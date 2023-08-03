#ifndef XFS_DISK_H

#define XFS_DISK_H

#include "fileSystem.h"
#include "exception.h"


#define DISK_NO_FORMAT 0
#define DISK_FORMAT 1

typedef struct
{
    char word[BLOCK_SIZE][WORD_SIZE];
} BLOCK;

void setPathToDisk(const char* path);
int readFromDisk(int virtBlockNumber, int fileBlockNumber);
int writeToDisk(int virtBlockNumber, int fileBlockNumber);
int openDiskFile(int access);
void createDiskFile(int format);
void diskCheckFileExists();

#endif
