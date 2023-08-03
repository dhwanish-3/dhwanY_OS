/*
Interface that handles files.
*/

#include "file.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Expands the ENV variables in path */
void file_expandPath(char *path)
{
    char *rem_path, *token;

    rem_path = strdup(path);
    token = strsep(&rem_path, "/");

    if (rem_path != NULL)
        sprintf(path, "%s/%s", getenv(++token) != NULL ? getenv(token) : token - 1, rem_path);
    else
        sprintf(path, "%s", getenv(++token) != NULL ? getenv(token) : token - 1);
}

/* Removes the file extension */
void file_remExtension(char *pathname)
{
    int i, l;

    l = strlen(pathname);
    i = l - 1;

    while (pathname[i] != '.' && i >= 0)
        i--;

    pathname[i + 1] = '\0';
}

/* Adds xsm extension */
void file_getOpFileName(char *dest, const char *inpfname)
{
    strcpy(dest, inpfname);
    file_remExtension(dest);
    strcat(dest, "xsm");
}