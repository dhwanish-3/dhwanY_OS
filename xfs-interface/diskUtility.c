/*
Interface to interact between XFS files and virtual disk.
*/

#include "diskUtility.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

extern BLOCK *disk;

/* Initialise disk */
void disk_init()
{
    virtual_disk_init();
}

/* Creates the disk file if not present */
void formatDisk(int format)
{
    int i, fd, rootFileDataBlocks[INODE_NUM_DATA_BLOCKS], userTableBase;

    if (format)
    {
        createDiskFile(DISK_NO_FORMAT);
        clearVirtDisk();

        setDefaultValues(DISK_FREE_LIST);
        commitMemCopyToDisk(DISK_FREE_LIST);

        setDefaultValues(INODE);
        setDefaultValues(ROOTFILE);

        for (i = 0; i < INODE_NUM_DATA_BLOCKS; i++)
        {
            if (i < NO_OF_ROOTFILE_BLOCKS)
                rootFileDataBlocks[i] = ROOTFILE + i;
            else
                rootFileDataBlocks[i] = -1;
        }

        AddEntryToMemInode(0, FILETYPE_ROOT, "root", NO_OF_ROOTFILE_BLOCKS * BLOCK_SIZE, rootFileDataBlocks);

        userTableBase = INODE * BLOCK_SIZE + 960;
        storeStringValueAt(userTableBase, "kernel");
        storeStringValueAt(userTableBase + 1, "-1");
        storeStringValueAt(userTableBase + 2, "root");
        storeStringValueAt(userTableBase + 3, "452");

        commitMemCopyToDisk(INODE);
        commitMemCopyToDisk(ROOTFILE);
    }
    else
        createDiskFile(DISK_NO_FORMAT);
}

/* Returns all the XFS files */
XOSFILE *getAllFiles()
{
    return _getAllFiles();
}

/* List all the XFS files */
void listAllFiles()
{
    XOSFILE *list, *next;

    list = getAllFiles();
    if (!list)
    {
        printf("The disk contains no files.\n");
        return;
    }

    while (list)
    {
        printf("Filename: %s \t Filesize %d\n", list->name, list->size);
        next = list->next;

        free(list->name);
        free(list);
        list = next;
    }
}

/* Display file contents */
void displayFileContents(char *name)
{
    int i, j, k, l, flag, locationOfInode;
    int blk[INODE_NUM_DATA_BLOCKS];

    flag = 0;
    diskCheckFileExists();

    for (i = 0; i < INODE_NUM_DATA_BLOCKS; i++)
        blk[i] = 0;

    locationOfInode = getInodeEntry(name);
    if (locationOfInode < 0)
    {
        printf("File \'%s\' not found!\n.", name);
        return;
    }

    getDataBlocks(blk, locationOfInode);

    k = 0;
    while (blk[k] > 0)
    {
        emptyBlock(TEMP_BLOCK);
        readFromDisk(TEMP_BLOCK, blk[k]);
        for (l = 0; l < BLOCK_SIZE; l++)
        {
            if (strcmp(disk[TEMP_BLOCK].word[l], "\0") != 0)
                printf("%s	\n", disk[TEMP_BLOCK].word[l]);
        }
        emptyBlock(TEMP_BLOCK);
        k++;
    }
}

/* Exports the contents of a XFS file to a UNIX file */
void exportFile(char *filename, char *unixfile)
{
    int i, j, k, l, flag, locationOfInode;
    int blk[INODE_NUM_DATA_BLOCKS];
    FILE *outFile;

    flag = 0;
    diskCheckFileExists();

    for (i = 0; i < INODE_NUM_DATA_BLOCKS; i++)
        blk[i] = 0;

    locationOfInode = getInodeEntry(filename);
    if (locationOfInode < 0)
    {
        printf("File \'%s\' not found!\n", filename);
        return;
    }

    getDataBlocks(blk, locationOfInode);
    expandpath(unixfile);

    outFile = fopen(unixfile, "w");
    if (outFile == NULL)
        printf("File \'%s\' not found!\n", unixfile);
    else
    {
        k = 0;
        while (blk[k] > 0)
        {
            emptyBlock(TEMP_BLOCK);
            readFromDisk(TEMP_BLOCK, blk[k]);

            for (l = 0; l < BLOCK_SIZE; l++)
                fprintf(outFile, "%s\n", disk[TEMP_BLOCK].word[l]);

            emptyBlock(TEMP_BLOCK);
            k++;
        }

        fclose(outFile);
    }
}

/* Clears the contents of the specified disk blocks */
int clearDiskBlocks(int disk_start_block, int no_of_disk_blocks)
{
    int i;

    emptyBlock(TEMP_BLOCK);

    for (i = 0; i < no_of_disk_blocks; i++)
        writeToDisk(TEMP_BLOCK, disk_start_block + i);

    return XFS_SUCCESS;
}

/* Copy the contents of the disk to a UNIX file */
int copyBlocksToFile(int startblock, int endblock, char *filename)
{
    int i, j;
    FILE *fp;

    diskCheckFileExists();
    expandpath(filename);

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("File \'%s\' not found!\n", filename);
        return XFS_FAILURE;
    }
    else
    {
        for (i = startblock; i <= endblock; i++)
        {
            emptyBlock(TEMP_BLOCK);
            readFromDisk(TEMP_BLOCK, i);

            for (j = 0; j < BLOCK_SIZE; j++)
                fprintf(fp, "%s\n", disk[TEMP_BLOCK].word[j]);
        }

        fclose(fp);
    }

    return XFS_SUCCESS;
}

/* Copies contents of a file to the specified block */
int writeFileToDisk(FILE *fp, int blockNum, int type)
{
    int i, j, line, line_count, flag, k;
    char buffer[32], s[16], temp[100], Instr[20], Arg1[20], Arg2[20], buffer1[16], c;
    char *instr, *arg1, *arg2, *string_start;

    line = 0;
    emptyBlock(TEMP_BLOCK);

    if (type == ASSEMBLY_CODE)
    {
        line_count = 0;
        flag = 0;
        k = 0;

        while (line_count < BLOCK_SIZE)
        {
            fgets(temp, 100, fp);

            if (feof(fp))
            {
                writeToDisk(TEMP_BLOCK, blockNum);
                return XFS_FAILURE;
            }

            string_start = strchr(temp, '"');
            if (string_start == NULL)
            {
                for (k = 0; k < 31; k++)
                    buffer[k] = temp[k];
                buffer[k] = '\0';
            }
            else
            {
                if (strlen(string_start) <= 16)
                {
                    for (k = 0; k < 31; k++)
                        buffer[k] = temp[k];
                    buffer[k] = '\0';
                }
                else
                {
                    for (k = 0; k < (strlen(temp) - strlen(string_start) + 15); k++)
                        buffer[k] = temp[k];
                    buffer[k - 1] = '"';
                    buffer[k] = '\0';
                }
            }

            if (strlen(buffer) > 1)
            {
                if (buffer[strlen(buffer) - 1] == '\n')
                    buffer[strlen(buffer) - 1] = '\0';

                instr = strtok(buffer, " ");
                arg1 = strtok(NULL, ",");
                arg2 = strtok(NULL, "\0");

                if (instr != NULL)
                {
                    strcpy(Instr, instr);
                    trim(Instr);
                }
                if (arg1 != NULL)
                {
                    strcpy(Arg1, arg1);
                    trim(Arg1);
                }
                if (arg2 != NULL)
                {
                    strcpy(Arg2, arg2);
                    trim(Arg2);
                }

                if (arg1 != NULL && arg2 != NULL)
                    arg1 = strcat(Arg1, ",");

                bzero(s, 16);

                if (instr != NULL)
                {
                    if (isdigit(Instr[0]))
                    {
                        sprintf(s, "%s", Instr);
                        strcpy(disk[TEMP_BLOCK].word[line_count], s);
                        line_count = line_count + 1;
                    }
                    else if (arg1 != NULL)
                    {
                        sprintf(s, "%s %s", Instr, Arg1);

                        for (j = strlen(s); j < 16; j++)
                            s[j] = '\0';

                        strcpy(disk[TEMP_BLOCK].word[line_count], s);

                        if (arg2 != NULL)
                        {
                            sprintf(s, "%s", Arg2);
                            strcpy(disk[TEMP_BLOCK].word[line_count + 1], s);
                        }
                        else
                        {
                            for (j = 0; j < 16; j++)
                                s[j] = '\0';
                            strcpy(disk[TEMP_BLOCK].word[line_count + 1], s);
                        }

                        line_count = line_count + 2;
                    }
                    else
                    {
                        sprintf(s, "%s", instr);

                        for (j = strlen(s); j <= 16; j++)
                            strcat(s, "\0");

                        strcpy(disk[TEMP_BLOCK].word[line_count], s);
                        bzero(s, 16);

                        for (j = 0; j < 16; j++)
                            s[j] = '\0';

                        strcpy(disk[TEMP_BLOCK].word[line_count + 1], s);
                        line_count = line_count + 2;
                    }
                }
            }
        }

        writeToDisk(TEMP_BLOCK, blockNum);
        return 1;
    }
    else if (type == DATA_FILE)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            fgets(buffer1, 16, fp);
	    for(j=0;buffer1[j]!='\0';j++)
		if(j>=1 && buffer1[j]=='\n')
			{
				buffer1[j]='\0';
				break;
			}
				
            strcpy(disk[TEMP_BLOCK].word[i], buffer1);

            if (feof(fp))
            {
                strcpy(disk[TEMP_BLOCK].word[i], "");
                writeToDisk(TEMP_BLOCK, blockNum);
                return XFS_FAILURE;
            }
        }

        writeToDisk(TEMP_BLOCK, blockNum);
        return 1;
    }

    return XFS_SUCCESS;
}

/* Returns the size of a unix data file in words */
int getDataFileSize(FILE *fp)
{
    int num_of_words;
    char buf[XSM_WORD_SIZE];

    num_of_words = 0;
    fseek(fp, 0, SEEK_SET);

    while (TRUE)
    {
        fgets(buf, XSM_WORD_SIZE, fp);
        num_of_words++;

        if (feof(fp))
            break;
    }

    return (num_of_words - 1);
}

void displayDiskFreeList()
{
    int i, j, no_of_free_blocks;

    no_of_free_blocks = 0;
    diskCheckFileExists();

    for (j = 0; j < NO_OF_FREE_LIST_BLOCKS; j++)
    {
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            printf("%d \t - \t %s  \n", i, disk[DISK_FREE_LIST + j].word[i]);

            if (getValue(disk[DISK_FREE_LIST + j].word[i]) == 0)
                no_of_free_blocks++;
        }
    }

    printf("\nNo of Free Blocks = %d", no_of_free_blocks);
    printf("\nTotal no of Blocks = %d\n", NO_OF_DISK_BLOCKS);
}

/* Trims whitespaces in a string */
void trim(char *str)
{
    char *start, *end;

    // Find first non-whitespace
    for (start = str; *start; start++)
    {
        if (!isspace((unsigned char)start[0]))
            break;
    }

    // Find start of last all-whitespace
    for (end = start + strlen(start); end > start + 1; end--)
    {
        if (!isspace((unsigned char)end[-1]))
            break;
    }

    *end = 0;

    // Shift from start to the beginning of the string
    if (start > str)
        memmove(str, start, (end - start) + 1);
}

/* Expand environment variables in path */
void expandpath(char *path)
{
    char *rem_path, *token;

    rem_path = strdup(path);
    token = strsep(&rem_path, "/");

    if (rem_path != NULL)
        sprintf(path, "%s/%s", getenv(++token) != NULL ? getenv(token) : token - 1, rem_path);
    else
        sprintf(path, "%s", getenv(++token) != NULL ? getenv(token) : token - 1);
}

/* Adds extemsion to filename */
void addext(char *filename, char *ext)
{
    int l = strlen(filename);

    if (l >= 16)
    {
        strcpy(filename + 11, ext);
        return;
    }

    if (strcmp(filename + l - 4, ext) != 0)
    {
        strcat(filename, ext);
        l = strlen(filename);

        if (l >= 16)
        {
            strcpy(filename + 11, ext);
            return;
        }
    }
}

/* Loads code from fileName to disk */
int loadCode(char *fileName, int disk_start_block, int no_of_disk_blocks)
{
    int i, j;
    FILE *fp;

    expandpath(fileName);

    fp = fopen(fileName, "r");
    if (fp == NULL)
    {
        printf("File %s not found.\n", fileName);
        return XFS_FAILURE;
    }

    for (i = 0; i < no_of_disk_blocks; i++)
    {
        j = writeFileToDisk(fp, disk_start_block + i, ASSEMBLY_CODE);
        if (j != 1)
            break;
    }

    if (j == 1)
    {
        printf("Code exceeds %d block\n", no_of_disk_blocks);
        clearDiskBlocks(disk_start_block, no_of_disk_blocks);
    }

    fclose(fp);
    return XFS_SUCCESS;
}

/* Loads code from fileName to disk after replacing labels */
int loadCodeWithLabels(char *infile, int disk_block, int no_of_disk_blocks, int mem_page)
{
    int n, ret;
    char fileName[66];

    expandpath(infile);

    labels_reset();
    labels_resolve(infile, fileName, mem_page * XSM_PAGE_SIZE);

    ret = loadCode(fileName, disk_block, no_of_disk_blocks);

    if (remove(fileName))
        printf("Error while trying to delete temporary file\n");

    return ret;
}

/* Loads the OS Startup code */
int loadOSCode(char *infile)
{
    return loadCodeWithLabels(infile, OS_STARTUP_CODE, OS_STARTUP_CODE_SIZE, MEM_OS_STARTUP_CODE);
}

/* Loads the Timer Interrupt Routine */
int loadTimerCode(char *infile)
{
    return loadCodeWithLabels(infile, TIMERINT, TIMERINT_SIZE, MEM_TIMERINT);
}

/* Loads the Disk Controller Routine */
int loadDiskControllerIntCode(char *infile)
{
    return loadCodeWithLabels(infile, DISKCONTROLLER_INT, DISKCONTROLLER_INT_SIZE, MEM_DISKCONTROLLER_INT);
}

/* Loads the Console Interrupt Routine */
int loadConsoleIntCode(char *infile)
{
    return loadCodeWithLabels(infile, CONSOLE_INT, CONSOLE_INT_SIZE, MEM_CONSOLE_INT);
}

/* Loads Exception Handler */
int loadExHandlerToDisk(char *infile)
{
    return loadCodeWithLabels(infile, EX_HANDLER, EX_HANDLER_SIZE, MEM_EX_HANDLER);
}

/* Loads the Interrupt Routines */
int loadIntCode(char *infile, int intNo)
{
    return loadCodeWithLabels(infile, ((intNo - 1) * INT_SIZE) + INT1, INT_SIZE, ((intNo - 1) * MEM_INT_SIZE) + MEM_INT1);
}

/* Loads the Module codes */
int loadModuleCode(char *infile, int modNo)
{
    return loadCodeWithLabels(infile, (modNo * MOD_SIZE) + MOD0, MOD_SIZE, (modNo * MEM_MOD_SIZE) + MEM_MOD0);
}

/* Loads the INIT program */
int loadINITCode(char *infile)
{
    return loadCode(infile, INIT_BLOCK, NO_OF_INIT_BLOCKS);
}

/* Loads the Idle program */
int loadIdleCode(char *infile)
{
    return loadCode(infile, IDLE_BLOCK, NO_OF_IDLE_BLOCKS);
}

/* Loads the Shell program */
int loadShellCode(char *infile)
{
    return loadCode(infile, SHELL_BLOCK, NO_OF_SHELL_BLOCKS);
}

/* Loads the Library */
int loadLibraryCode(char *infile)
{
    return loadCode(infile, LIBRARY_BLOCK, NO_OF_LIBRARY_BLOCKS);
}

/* Loads Data files */
int loadDataToDisk(char *name)
{
    int i, j, k, num_of_chars, num_of_blocks_reqd, num_of_words, file_size;
    int freeBlock[INODE_MAX_BLOCK_NUM];
    char c, *s, filename[50];
    FILE *fileToBeLoaded;

    num_of_chars = 0;
    num_of_blocks_reqd = 0;
    file_size = 0;

    for (i = 0; i < INODE_MAX_BLOCK_NUM; i++)
        freeBlock[i] = -1;

    c = '\0';
    s = strrchr(name, '/');
    if (s != NULL)
        strcpy(filename, s + 1);
    else
        strcpy(filename, name);

    filename[15] = '\0';
    addext(filename, ".dat");
    expandpath(name);

    fileToBeLoaded = fopen(name, "r");
    if (fileToBeLoaded == NULL)
    {
        printf("File \'%s\' not found.!\n", name);
        return XFS_FAILURE;
    }

    fseek(fileToBeLoaded, 0L, SEEK_END);

    num_of_chars = ftell(fileToBeLoaded);
    num_of_words = getDataFileSize(fileToBeLoaded);
    num_of_blocks_reqd = (num_of_words / 512) + ((num_of_words % 512 == 0) ? 0 : 1);

    if (num_of_blocks_reqd > INODE_MAX_BLOCK_NUM)
    {
        printf("The size of file exceeds %d blocks\n", INODE_MAX_BLOCK_NUM);
        printf("The file contains %d words, an xfs file can have only upto %d words\n", num_of_words, INODE_MAX_BLOCK_NUM * BLOCK_SIZE);
        return XFS_FAILURE;
    }

    fseek(fileToBeLoaded, 0, SEEK_SET);

    for (i = 0; i < num_of_blocks_reqd; i++)
    {
        if ((freeBlock[i] = FindFreeBlock()) == XFS_FAILURE)
        {
            printf("Disk does not have enough space to contain the file.\n");
            freeUnusedBlock(freeBlock, INODE_MAX_BLOCK_NUM);
            return XFS_FAILURE;
        }
    }

    i = getInodeEntry(filename);
    if (i >= 0)
    {
        printf("Disk already contains the file with this name. Try again with a different name.\n");
        freeUnusedBlock(freeBlock, INODE_MAX_BLOCK_NUM);
        return XFS_FAILURE;
    }

    k = FindEmptyInodeEntry();

    if (k == XFS_FAILURE)
    {
        freeUnusedBlock(freeBlock, INODE_MAX_BLOCK_NUM);
        printf("No free INODE entry found.\n");
        return XFS_FAILURE;
    }

    commitMemCopyToDisk(DISK_FREE_LIST);
    emptyBlock(TEMP_BLOCK);

    for (i = 0; i < num_of_blocks_reqd; i++)
    {
        j = writeFileToDisk(fileToBeLoaded, freeBlock[i], DATA_FILE);
        file_size++;
    }

    AddEntryToMemInode(k, FILETYPE_DATA, filename, num_of_words, freeBlock);
    commitMemCopyToDisk(INODE);

    fclose(fileToBeLoaded);
    return XFS_SUCCESS;
}

/* Loads Executable files */
int loadExecutableToDisk(char *name)
{
    int i, j, k, l, file_size, num_of_lines, num_of_blocks_reqd;
    int freeBlock[INODE_MAX_BLOCK_NUM];
    char c, *s, filename[50];
    FILE *fileToBeLoaded;

    file_size = 0;
    num_of_lines = 0;
    num_of_blocks_reqd = 0;

    for (i = 0; i < INODE_MAX_BLOCK_NUM; i++)
        freeBlock[i] = -1;

    c = '\0';
    s = strrchr(name, '/');

    if (s != NULL)
        strcpy(filename, s + 1);
    else
        strcpy(filename, name);

    filename[15] = '\0';
    addext(filename, ".xsm");
    expandpath(name);

    fileToBeLoaded = fopen(name, "r");
    if (fileToBeLoaded == NULL)
    {
        printf("File %s not found.\n", name);
        return XFS_FAILURE;
    }

    while (c != EOF)
    {
        c = fgetc(fileToBeLoaded);
        if (c == '\n')
            num_of_lines++;
    }

    num_of_blocks_reqd = (num_of_lines / (BLOCK_SIZE / 2)) + 1;
    if (num_of_blocks_reqd > INODE_MAX_BLOCK_NUM)
    {
        printf("The size of file exceeds %d blocks", INODE_MAX_BLOCK_NUM);
        return XFS_FAILURE;
    }

    fseek(fileToBeLoaded, 0, SEEK_SET);

    for (i = 0; i < num_of_blocks_reqd; i++)
    {
        if ((freeBlock[i] = FindFreeBlock()) == -1)
        {
            printf("Insufficient disk space!\n");
            freeUnusedBlock(freeBlock, INODE_MAX_BLOCK_NUM);
            return XFS_FAILURE;
        }
    }

    i = getInodeEntry(filename);
    if (i >= 0)
    {
        printf("Disk already contains the file with this name. Try again with a different name.\n");
        freeUnusedBlock(freeBlock, INODE_MAX_BLOCK_NUM);
        return -1;
    }

    k = FindEmptyInodeEntry();
    if (k == XFS_FAILURE)
    {
        freeUnusedBlock(freeBlock, INODE_MAX_BLOCK_NUM);
        printf("No free INODE entry found.\n");
        return XFS_FAILURE;
    }

    commitMemCopyToDisk(DISK_FREE_LIST);
    emptyBlock(TEMP_BLOCK);

    for (i = 0; i < num_of_blocks_reqd; i++)
    {
        j = writeFileToDisk(fileToBeLoaded, freeBlock[i], ASSEMBLY_CODE);
        file_size++;
    }

    AddEntryToMemInode(k, FILETYPE_EXEC, filename, num_of_lines * 2, freeBlock);
    commitMemCopyToDisk(INODE);

    fclose(fileToBeLoaded);
    return XFS_SUCCESS;
}

/* Deletes the OS Startup code */
int deleteOSCodeFromDisk()
{
    return clearDiskBlocks(OS_STARTUP_CODE, OS_STARTUP_CODE_SIZE);
}

/* Deletes the Timer Interrupt Routine */
int deleteTimerFromDisk()
{
    return clearDiskBlocks(TIMERINT, TIMERINT_SIZE);
}

/* Deletes the Disk Controller Routine */
int deleteDiskControllerINTFromDisk()
{
    return clearDiskBlocks(DISKCONTROLLER_INT, DISKCONTROLLER_INT_SIZE);
}

/* Deletes the Console Interrupt Routine */
int deleteConsoleINTFromDisk()
{
    return clearDiskBlocks(CONSOLE_INT, CONSOLE_INT_SIZE);
}

/* Deletes the Exception Handler */
int deleteExHandlerFromDisk()
{
    return clearDiskBlocks(EX_HANDLER, EX_HANDLER_SIZE);
}

/* Deletes the Interrupt Routines */
int deleteIntCodeFromDisk(int intNo)
{
    return clearDiskBlocks(((intNo - 1) * INT_SIZE) + INT1, INT_SIZE);
}

/* Deletes the Module codes */
int deleteModuleCodeFromDisk(int modNo)
{
    return clearDiskBlocks((modNo * MOD_SIZE) + MOD0, MOD_SIZE);
}

/* Deletes the INIT program */
int deleteINITFromDisk()
{
    return clearDiskBlocks(INIT_BLOCK, NO_OF_INIT_BLOCKS);
}

/* Deletes the Idle program */
int deleteIdleFromDisk()
{
    return clearDiskBlocks(IDLE_BLOCK, NO_OF_IDLE_BLOCKS);
}

/* Deletes the Shell program */
int deleteShellFromDisk()
{
    return clearDiskBlocks(SHELL_BLOCK, NO_OF_SHELL_BLOCKS);
}

/* Deletes the Lirary program */
int deleteLibraryFromDisk()
{
    return clearDiskBlocks(LIBRARY_BLOCK, NO_OF_LIBRARY_BLOCKS);
}

/* Deletes an executable/data file from the disk */
int deleteFileFromDisk(char *name)
{
    int i, max_num_blocks, locationOfInode;
    int blockAddresses[INODE_MAX_BLOCK_NUM];

    if (strcmp(name, "root") == 0)
    {
        printf("Root file cannot be deleted\n");
        return XFS_FAILURE;
    }

    diskCheckFileExists();

    max_num_blocks = INODE_MAX_BLOCK_NUM;

    for (i = 0; i < max_num_blocks; i++)
        blockAddresses[i] = 0;

    locationOfInode = getInodeEntry(name);
    if (locationOfInode < 0)
    {
        printf("File \'%s\' not found!\n", name);
        return -1;
    }

    getDataBlocks(blockAddresses, locationOfInode);
    freeUnusedBlock(blockAddresses, max_num_blocks);

    removeInodeEntry(locationOfInode);

    commitMemCopyToDisk(INODE);
    commitMemCopyToDisk(DISK_FREE_LIST);

    return XFS_SUCCESS;
}

/* Copies the contents of Root file to a UNIX file */
int dumpRootFile(const char *filename)
{
    return copyBlocksToFile(ROOTFILE, ROOTFILE + NO_OF_ROOTFILE_BLOCKS - 1, strdup(filename));
}

/* Copies the contents of Inode table to a UNIX file */
int dumpInodeTable(const char *filename)
{
    return copyBlocksToFile(INODE, INODE + NO_OF_INODE_BLOCKS - 1, strdup(filename));
}
