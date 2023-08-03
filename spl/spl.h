#include <stdio.h>

#include "data.h"
#include "file.h"
#include "label.h"
#include "node.h"
#include "y.tab.h"

#define LEGAL 0
#define PAGE_PER_INTERRUPT 2
#define PAGE_SIZE 512
#define CONSTANT_NAME_MAX_LEN 30

extern int linecount;
extern int out_linecount;
extern int flag_break;
extern int regcount;
extern int depth;
extern unsigned long temp_pos;
extern FILE *fp;

struct define
{
    char name[CONSTANT_NAME_MAX_LEN];
    int value;
    struct define *next;
};
extern struct define *root_define;
struct define *lookup_constant(char *name);

struct alias
{
    char name[CONSTANT_NAME_MAX_LEN];
    int reg, depth;
    struct alias *next;
};
extern struct alias *root_alias;

struct define* lookup_constant(char *name);
struct alias *lookup_alias(char *name);
struct alias *lookup_alias_reg(int reg);
void push_alias(char *name, int reg);
void pop_alias();
void insert_constant(char *name, int value);
void add_predefined_constants();
node *substitute_id(node *id);
void getreg(node *root, char reg[]);
void codegen(node *root);

int yylex();
int yyparse();
int yyerror();