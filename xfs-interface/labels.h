#ifndef XFS_LABELS_H

#define XFS_LABELS_H

#include "constants.h"

#include <stdio.h>

typedef struct _label
{
    char *name;
    int address;
    struct _label *next;
} label;

void labels_reset();
int labels_is_label(const char *str);
int labels_is_charstring(char *str);
char *labels_get_name(char *label);
int labels_get_target(const char *name);
void labels_insert(char *label_name, int address);
int labels_resolve(const char *filename, char *outfile, int base_address);
int labels_phase_one(FILE *fp);
int labels_phase_two(FILE *fin, FILE *fout, int base_address);
void labels_random_name(char *name);
int remove_newline_character(char *str, int length);

#endif
