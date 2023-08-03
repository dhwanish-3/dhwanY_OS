#ifndef SPL_FILE_H

#define SPL_FILE_H

#define FILENAME_MAX_LEN 200

void file_expandPath(char *path);
void file_remExtension(char *pathname);
void file_getOpFileName(char *dest, const char *inpfname);

#endif