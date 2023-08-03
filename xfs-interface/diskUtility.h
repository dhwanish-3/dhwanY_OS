#ifndef XFS_DISKUTIL_H

#define XFS_DISKUTIL_H

#include "constants.h"
#include "inode.h"
#include "labels.h"
#include "memOrg.h"
#include "virtualDisk.h"

#include <stdio.h>

#define ASSEMBLY_CODE 0
#define DATA_FILE 1

#define XFS_ERROR -1

void disk_init();
void formatDisk(int format);
XOSFILE *getAllFiles();
void listAllFiles();
void displayFileContents(char *name);
void exportFile(char *filename, char *unixfile);
int clearDiskBlocks(int disk_start_block, int no_of_disk_blocks);
int copyBlocksToFile(int startblock, int endblock, char *filename);
int writeFileToDisk(FILE *fp, int blockNum, int type);
int getDataFileSize(FILE *fp);
void displayDiskFreeList();
void trim(char *str);
void expandpath(char *path);
void addext(char *filename, char *ext);
int loadCode(char *fileName, int disk_start_block, int no_of_disk_blocks);
int loadCodeWithLabels(char *infile, int disk_block, int no_of_disk_blocks, int mem_page);
int loadOSCode(char *infile);
int loadTimerCode(char *infile);
int loadDiskControllerIntCode(char *infile);
int loadConsoleIntCode(char *infile);
int loadExHandlerToDisk(char *infile);
int loadIntCode(char *infile, int intNo);
int loadModuleCode(char *infile, int modNo);
int loadINITCode(char *infile);
int loadIdleCode(char *infile);
int loadShellCode(char *infile);
int loadLibraryCode(char *infile);
int loadDataToDisk(char *name);
int loadExecutableToDisk(char *name);
int deleteOSCodeFromDisk();
int deleteTimerFromDisk();
int deleteDiskControllerINTFromDisk();
int deleteConsoleINTFromDisk();	
int deleteExHandlerFromDisk();
int deleteIntCodeFromDisk(int intNo);
int deleteModuleCodeFromDisk(int modNo);
int deleteINITFromDisk();
int deleteIdleFromDisk();
int deleteShellFromDisk();
int deleteLibraryFromDisk();
int deleteFileFromDisk(char *name);
void displayDiskFreeList();
int dumpRootFile(const char *filename);
int dumpInodeTable(const char *filename);

#endif
