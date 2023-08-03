/*
Interface to access disk file.
*/

#include "disk.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

BLOCK *disk;
/* path to disk.xfs */
static const char* pathToDisk = NULL;

void setPathToDisk(const char* path) {
    pathToDisk = path;
}


/* Reads an entire block from fileBlockNumber on the disk to virtBlockNumber on the memory copy of the disk */
int readFromDisk(int virtBlockNumber, int fileBlockNumber)
{
    int fd = openDiskFile(O_RDONLY);

    lseek(fd, sizeof(BLOCK) * fileBlockNumber, SEEK_SET);
    read(fd, &disk[virtBlockNumber], sizeof(BLOCK));
    close(fd);

    return XFS_SUCCESS;
}

/* Writes an entire block to fileBlocknumber on the disk from virtBlockNumber on the memory copy of the disk */
int writeToDisk(int virtBlockNumber, int fileBlockNumber)
{
    int fd = openDiskFile(O_WRONLY);

    lseek(fd, 0, SEEK_SET);
    lseek(fd, sizeof(BLOCK) * fileBlockNumber, SEEK_CUR);
    write(fd, &disk[virtBlockNumber], sizeof(BLOCK));
    close(fd);

    return XFS_SUCCESS;
}

/* Opens the disk file and returns the file descriptor */
int openDiskFile(int access)
{
    int fd = open(pathToDisk, access, 0666);
    if (fd < 0)
        exception_throwException(EXCEPTION_CANT_OPEN_DISK);

    return fd;
}

/* Creates the disk file */
void createDiskFile(int format)
{
    int fd;

    if (format == DISK_FORMAT)
        fd = open(pathToDisk, O_CREAT | O_TRUNC | O_SYNC, 0666);
    else
        fd = open(pathToDisk, O_CREAT, 0666);

    if (fd < 0)
        exception_throwException(EXCEPTION_CANT_CREATE_DISK);

    close(fd);
}

/* Tries to open the disk */
void diskCheckFileExists()
{
    close(openDiskFile(O_RDONLY));
}
