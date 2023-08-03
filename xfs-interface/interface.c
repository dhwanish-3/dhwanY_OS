/*
User Interface.
*/

#include "interface.h"

#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

// For command completion
#include <readline/readline.h>
#include <readline/history.h>

jmp_buf exp_point;

/* Invokes Command Line Interface */
void cli(int argc, char **argv)
{
    int i, j;
    char command[100], temp[100], c;

    if (argc > 1)
    {
        strcpy(command, argv[1]);

        i = 2;
        while (i < argc)
        {
            strcpy(temp, command);
            sprintf(command, "%s %s", temp, argv[i]);
            i++;
        }

        runCommand(command);
    }
    else
    {
        printf("Unix-XFS Interace Version 2.0. \nType \"help\" for getting a list of commands.\n");

        cli_init_completion();
        cli_run_with_completion();
    }
}

/* Runs the XFS Interface command */
void runCommand(char command[])
{
    int exp_occured, intNo, startBlock, endBlock;
    char *name, *arg1, *arg2, *arg3, *line, *c;
    char *int_command, *intType, *fileName;
    FILE *fp;

    name = strtok(command, " ");
    line = NULL;
    exp_occured = setjmp(exp_point);

    if (exp_occured)
    {
        exception_printErrorMessage(exp_occured);
        return;
    }

    if (strcmp(name, "help") == 0)
    {
        printf(" fdisk \n\t Format the disk with XFS filesystem\n");
        printf(" run <pathname> \n\t Executes the set of xfs-interface commands sequentially \n");
        printf(" load --exec <pathname> \n\t Loads an executable file to XFS disk \n");
        printf(" load --data <pathname> \n\t Loads a data file to XFS disk \n");
        printf(" load --init <pathname> \n\t Loads INIT code to XFS disk \n");
        printf(" load --os <pathname> \n\t Loads OS startup code to XFS disk \n");
        printf(" load --idle <pathname> \n\t Loads Idle code to XFS disk \n");
        printf(" load --shell <pathname> \n\t Loads Shell code to XFS disk \n");
        printf(" load --library <pathname> \n\t Loads Library code to XFS disk \n");
        printf(" load --int=timer <pathname>\n\t Loads Timer Interrupt routine to XFS disk \n");
        printf(" load --int=disk <pathname>\n\t Loads Disk Controller Interrupt routine to XFS disk \n");
        printf(" load --int=console <pathname>\n\t Loads Console Interrupt routine to XFS disk \n");
        printf(" load --int=[4-18] <pathname>\n\t Loads the specified Interrupt routine to XFS disk \n");
        printf(" load --exhandler <pathname> \n\t Loads exception handler routine to XFS disk \n");
        printf(" load --module [0-7] <pathname>\n\t Loads the specified Module to XFS disk \n");
        printf(" export <xfs_filename> <pathname>\n\t Exports a data file from XFS disk to UNIX file system\n");
        printf(" rm <xfs_filename>\n\t Removes a file from XFS disk \n");
        printf(" ls \n\t List all files\n");
        printf(" df \n\t Display free list and free space\n");
        printf(" cat <xfs_filename> \n\t to display contents of a file\n");
        printf(" copy <start_blocks> <end_block> <unix_filename> \n\t Copies contents of specified range of blocks to a UNIX file.\n");
        printf(" dump --inodeusertable\n\t Copies the contents of inode table and the user table to an external UNIX file named inodeusertable.txt\n");
        printf(" dump --rootfile \n\t Copies the contents of root file to an external UNIX file named rootfile.txt\n");
        printf(" exit \n\t Exit the interface\n");
    }
    else if (strcmp(name, "fdisk") == 0)
    {
        printf("Formatting Complete. \"disk.xfs\" created.\n");
        formatDisk(FORMAT);
    }
    else if (strcmp(name, "run") == 0)
    {
        arg1 = strtok(NULL, " ");
        fp = fopen(arg1, "r");
        size_t len = 0;

        if (fp == NULL)
            printf("Unable to open file : %s.\n", arg1);
        else
        {
            while (getline(&line, &len, fp) != -1)
            {
                if (line[strlen(line) - 1] == '\n')
                    line[strlen(line) - 1] = '\0';
                runCommand(line);
            }
            fclose(fp);
        }
    }
    else if (strcmp(name, "load") == 0)
    {
        arg1 = strtok(NULL, " ");
        arg2 = strtok(NULL, " ");
        arg3 = strtok(NULL, " ");

        int_command = strtok(arg1, "=");
        intType = strtok(NULL, "=");
        fileName = (char *)malloc(101 * sizeof(char));

        if (!arg2)
            fileName = NULL;
        else
            strncpy(fileName, arg2, 100);

        if (fileName != NULL)
            fileName[100] = '\0';
        else
        {
            printf("Missing <pathname> for load. See \"help\" for more information.\n");
            return;
        }

        if (strcmp(arg1, "--exec") == 0)
        {
            if (strlen(basename(fileName)) > 12)
            {
                printf("Filename is more than 12 characters long.\n");
                return;
            }

            c = strrchr(fileName, '.');
            if (c == NULL || strcmp(c, ".xsm") != 0)
            {
                printf("Filename does not have \".xsm\" extension.\n");
                return;
            }

            loadExecutableToDisk(fileName);
        }
        else if (strcmp(arg1, "--init") == 0)
            loadINITCode(fileName);
        else if (strcmp(arg1, "--shell") == 0)
            loadShellCode(fileName);
        else if (strcmp(arg1, "--library") == 0)
            loadLibraryCode(fileName);
        else if (strcmp(arg1, "--idle") == 0)
            loadIdleCode(fileName);
        else if (strcmp(arg1, "--data") == 0)
        {
            if (strlen(basename(fileName)) > 12)
            {
                printf("Filename is more than 12 characters long.\n");
                return;
            }

            c = strrchr(fileName, '.');
            if (c == NULL || strcmp(c, ".dat") != 0)
            {
                printf("Filename does not have \".dat\" extension.\n");
                return;
            }

            loadDataToDisk(fileName);
        }
        else if (strcmp(arg1, "--os") == 0)
            loadOSCode(fileName);
        else if (strcmp(arg1, "--int") == 0)
        {
            if (strcmp(intType, "timer") == 0)
                loadTimerCode(fileName);
            else if (strcmp(intType, "disk") == 0)
                loadDiskControllerIntCode(fileName);
            else if (strcmp(intType, "console") == 0)
                loadConsoleIntCode(fileName);
            else
            {
                int intNo = atoi(intType);
                if (intNo >= 4 && intNo <= NO_OF_INTERRUPTS)
                    loadIntCode(fileName, intNo);
                else
                {
                    printf("Invalid argument for \"--int=\".\n");
                    return;
                }
            }
        }
        else if (strcmp(arg1, "--module") == 0)
        {
            intNo = atoi(arg2);
            if (intNo >= 0 && intNo <= NO_OF_MODULES)
                loadModuleCode(arg3, intNo);
            else
            {
                printf("Invalid argument for \"--module=\".\n");
                return;
            }
        }
        else if (strcmp(arg1, "--exhandler") == 0)
            loadExHandlerToDisk(fileName);
        else
            printf("Invalid argument \"%s\" for load. See \"help\" for more information.\n", arg1);

        free(fileName);
    }
    else if (strcmp(name, "rm") == 0)
    {
        arg1 = strtok(NULL, " ");
        fileName = arg1;

        if (fileName != NULL)
            deleteFileFromDisk(arg1);
        else
            printf("Missing <xfs_filename> for rm. See \"help\" for more information.\n");
    }
    else if (strcmp(name, "export") == 0)
    {
        arg1 = strtok(NULL, " ");
        arg2 = strtok(NULL, " ");

        if (arg2 != NULL)
            exportFile(arg1, arg2);
        else
        {
            printf("Missing <pathname> for export. See \"help\" for more information.\n");
            return;
        }
    }
    else if (strcmp(name, "ls") == 0)
        listAllFiles();
    else if (strcmp(name, "df") == 0)
        displayDiskFreeList();
    else if (strcmp(name, "cat") == 0)
    {
        arg1 = strtok(NULL, " ");
        fileName = arg1;

        if (fileName != NULL)
            displayFileContents(fileName);
        else
        {
            printf("Missing <xfs_filename> for cat. See \"help\" for more information.\n");
            return;
        }
    }
    else if (strcmp(name, "copy") == 0)
    {
        arg1 = strtok(NULL, " ");
        arg2 = strtok(NULL, " ");
        arg3 = strtok(NULL, " ");

        if (arg1 == NULL || arg2 == NULL || arg3 == NULL)
        {
            printf("Insufficient arguments for \"copy\". See \"help\" for more information.\n");
            return;
        }
        else
        {
            startBlock = atoi(arg1);
            endBlock = atoi(arg2);
            fileName = arg3;
            fileName[50] = '\0';

            copyBlocksToFile(startBlock, endBlock, fileName);
        }
    }
    else if (strcmp(name, "dump") == 0)
    {
        arg1 = strtok(NULL, " ");
        if (strcmp(arg1, "--inodeusertable") == 0)
            dumpInodeTable("inodeusertable.txt");
        else if (strcmp(arg1, "--rootfile") == 0)
            dumpRootFile("rootfile.txt");
        else
            printf("Invalid argument \"%s\" for dump. See \"help\" for more information.\n", arg1);
    }
    else if (strcmp(name, "exit") == 0)
        exit(0);
    else
        printf("Unknown command \"%s\". See \"help\" for more information.\n", name);
}

/* Strips whitespaces */
char *xfs_cli_stripwhite(char *str)
{
    char *end = str + strlen(str) - 1;

    while (isspace(*str))
        str++;

    while (isspace(*end) && str <= end)
        end--;

    *++end = '\0';
    return str;
}

/* Sets up auto-complete mechanism */
void cli_init_completion()
{
    rl_readline_name = "xfs-interface";
    rl_attempted_completion_function = xfs_cli_completion;
}

/* Command line interface with command completion */
void cli_run_with_completion()
{
    char *line_read, *command;

    line_read = NULL;

    while (TRUE)
    {
        // Return the memory to the pool if needed
        if (line_read)
        {
            free(line_read);
            line_read = NULL;
        }

        line_read = readline("# ");
        if (!line_read)
            continue;

        command = xfs_cli_stripwhite(line_read);
        if (!strlen(command))
            continue;

        add_history(command);
        if (!strcmp(command, "exit"))
            break;

        runCommand(command);
    }
}

/* XFS CLI commands completion */
char **xfs_cli_completion(const char *text, int start, int end)
{
    char **matches, *curr_context, *pch;

    curr_context = malloc(start + 1);
    strncpy(curr_context, rl_line_buffer, start);

    curr_context[start] = '\0';
    pch = strtok(curr_context, " ");
    matches = NULL;
    rl_completion_append_character = ' ';

    if (pch == NULL)
        matches = rl_completion_matches(text, xfs_cli_command_gen);
    else if (!strcmp(pch, "load"))
    {
        if (start >= 6 &&
            rl_line_buffer[start - 6] == '-' &&
            rl_line_buffer[start - 5] == '-' &&
            rl_line_buffer[start - 4] == 'i' &&
            rl_line_buffer[start - 3] == 'n' &&
            rl_line_buffer[start - 2] == 't' &&
            rl_line_buffer[start - 1] == '=')
            matches = rl_completion_matches(text, xfs_cli_int_gen);
        else
        {
            pch = strtok(NULL, " ");
            if (pch != NULL && !strcmp(pch, "--module"))
                matches = rl_completion_matches(text, xfs_cli_module_gen);
            else
                matches = rl_completion_matches(text, xfs_cli_opt_gen);
        }
    }
    else if (!strcmp(pch, "export"))
        matches = rl_completion_matches(text, xfs_cli_file_gen);
    else if (!strcmp(pch, "dump"))
        matches = rl_completion_matches(text, xfs_cli_dump_gen);
    else if (!strcmp(pch, "cat") || !strcmp(pch, "rm"))
        matches = rl_completion_matches(text, xfs_cli_file_gen);

    if (matches == NULL)
        rl_completion_append_character = '\0';

    free(curr_context);
    return matches;
}

/* XFS command generation */
char *xfs_cli_command_gen(const char *text, int state)
{
    static int index, len;
    const int comm_len = 12;
    const char *commands[12] = {"fdisk", "run", "load", "export", "rm", "ls", "df", "cat", "copy", "dump", "exit", "help"};

    if (state == 0)
    {
        index = 0;
        len = strlen(text);
    }

    for (; index < comm_len; ++index)
        if (!strncmp(text, commands[index], len))
            return strdup(commands[index++]);

    return NULL;
}

/* XFS options generation */
char *xfs_cli_opt_gen(const char *text, int state)
{
    static int index, len;
    const int opt_len = 10;
    const char *options[10] = {"--int=", "--exec", "--data", "--init", "--os", "--idle", "--shell", "--library", "--exhandler", "--module"};

    if (state == 0)
    {
        index = 0;
        len = strlen(text);
    }

    for (; index < opt_len; ++index)
    {
        if (!strncmp(text, options[index], len))
        {
            // Prevent readline from appending a space after possible --int=
            if (index == 0)
                rl_completion_append_character = '\0';
            return strdup(options[index++]);
        }
    }

    return NULL;
}

/* XFS file generation */
char *xfs_cli_file_gen(const char *text, int state)
{
    static int i, j, len;
    static XOSFILE *files;
    char *result = NULL;
    XOSFILE *next;

    if (state == 0)
    {
        xfs_cli_destroy_file_list(files);
        files = getAllFiles();
        len = strlen(text);
    }

    while (files)
    {
        if (!strncmp(text, files->name, len))
            result = strdup(files->name);

        next = files->next;
        free(files->name);
        free(files);
        files = next;

        if (result)
            break;
    }

    return result;
}

/* XFS int generation */
char *xfs_cli_int_gen(const char *text, int state)
{
    static int index, len;
    const int ints_len = 18;
    const char *ints[18] = {"4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "timer", "disk", "console"};

    if (state == 0)
    {
        index = 0;
        len = strlen(text);
    }

    for (; index < ints_len; ++index)
        if (!strncmp(text, ints[index], len))
            return strdup(ints[index++]);

    return NULL;
}

/* XFS module generation */
char *xfs_cli_module_gen(const char *text, int state)
{
    static int index, len;
    const int opt_len = 8;
    const char *options[8] = {"0", "1", "2", "3", "4", "5", "6", "7"};

    if (state == 0)
    {
        index = 0;
        len = strlen(text);
    }

    for (; index < opt_len; ++index)
        if (!strncmp(text, options[index], len))
            return strdup(options[index++]);

    return NULL;
}

/* XFS dump generation */
char *xfs_cli_dump_gen(const char *text, int state)
{
    static int index, len;
    const int opt_len = 2;
    const char *options[2] = {"--inodeusertable", "--rootfile"};

    if (state == 0)
    {
        index = 0;
        len = strlen(text);
    }

    for (; index < opt_len; ++index)
        if (!strncmp(text, options[index], len))
            return strdup(options[index++]);

    return NULL;
}

/* Frees the files list */
static void xfs_cli_destroy_file_list(XOSFILE *files)
{
    XOSFILE *curr_ptr, *next_ptr;

    curr_ptr = files;
    while (curr_ptr)
    {
        free(curr_ptr->name);
        next_ptr = curr_ptr->next;

        free(curr_ptr);
        curr_ptr = next_ptr;
    }
}

int main(int argc, char **argv)
{
    int intNo, fd;

    disk_init();

    const char* disk_file = DEFAULT_DISK_NAME;

    if(argc>1 && strcmp(argv[1], "--disk-file")==0)
    {
        argc --;
        argv ++;
        if(argc>1)
        {
            disk_file = argv[1];
            argc --;
            argv ++;
        }
        else
        {
            printf("--disk-file option requires a file name.\n");
            printf("\n");
            printf("Syntax: --disk-file /path/to/disk.xfs\n");
            printf("Specifies the path to disk.xfs to use.\n");
            exit(-1);
        }
    }

    setPathToDisk(disk_file);

    fd = open(disk_file, O_RDONLY, 0666);
    if (fd > 0)
    {
        close(fd);
        loadFileToVirtualDisk();
    }

    cli(argc, argv);
    return 0;
}
