/*
The SPL interface.
*/

#include "spl.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern FILE *yyin;

int out_linecount = 0;
int flag_break = 0;
int regcount = 0;
int depth = 0;
FILE *fp;
struct define *root_define = NULL;
struct alias *root_alias = NULL;

/* Returns the constant */
struct define *lookup_constant(char *name)
{
    struct define *temp = root_define;
    while (temp != NULL)
    {
        if (strcmp(name, temp->name) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

/* Returns the alias */
struct alias *lookup_alias(char *name)
{
    struct alias *temp = root_alias;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
            return (temp);
        temp = temp->next;
    }

    return NULL;
}

/* Returns the register alias */
struct alias *lookup_alias_reg(int reg)
{
    struct alias *temp = root_alias;
    while (temp != NULL)
    {
        if (reg == temp->reg)
            return (temp);
        temp = temp->next;
    }

    return NULL;
}

/* Push a new alias to the list */
void push_alias(char *name, int reg)
{
    struct alias *temp;

    if (lookup_constant(name) != NULL)
    {
        printf("\n%d: Alias name %s already used as symbolic contant!!\n", linecount, name);
        exit(0);
    }

    temp = lookup_alias(name);
    if (temp != NULL && temp->depth == depth)
    {
        printf("\n%d: Alias name %s already used as in the current block!!\n", linecount, name);
        exit(0);
    }
    else
    {
        temp = lookup_alias_reg(reg);
        if (temp != NULL && temp->depth == depth)
            strcpy(temp->name, name);
        else
        {
            temp = malloc(sizeof(struct alias));
            strcpy(temp->name, name);
            temp->reg = reg;
            temp->depth = depth;
            temp->next = root_alias;
            root_alias = temp;
        }
    }
}

/* Pop a alias from the list */
void pop_alias()
{
    struct alias *temp;

    temp = root_alias;
    while (temp != NULL && temp->depth == depth)
    {
        root_alias = temp->next;
        free(temp);
        temp = root_alias;
    }
}

/* Add a new constant to the list */
void insert_constant(char *name, int value)
{
    struct define *temp;

    temp = lookup_constant(name);
    if (temp == NULL)
    {
        temp = malloc(sizeof(struct define));
        strcpy(temp->name, name);
        temp->value = value;
        temp->next = root_define;
        root_define = temp;
    }
    else
    {
        printf("\n%d: Multiple Definitions for constant %s!!\n", linecount, name);
        exit(0);
    }
}

/* Add the pre-defined constants to the list */
void add_predefined_constants()
{
    int value;
    char name[CONSTANT_NAME_MAX_LEN];
    FILE *c_fp;

    c_fp = fopen("splconstants.cfg", "r");
    if (!c_fp)
    {
        printf("\nUnable to open splconstants.cfg file!\nExiting\n");
        exit(0);
    }

    while (!feof(c_fp))
    {
        bzero(name, CONSTANT_NAME_MAX_LEN);
        if (fscanf(c_fp, "%s %d", name, &value) == 2)
            insert_constant(name, value);
        else
            break;
    }

    fclose(c_fp);
}

/* Substitute alias with constant or register */
node *substitute_id(node *id)
{
    struct define *temp;
    struct alias *temp2;

    temp = lookup_constant(id->name);
    if (temp != NULL)
    {
        id->nodetype = NODE_NUM;
        id->name = NULL;
        id->value = temp->value;
        return (id);
    }

    temp2 = lookup_alias(id->name);
    if (temp2 == NULL)
    {
        printf("\n%d: Unknown identifier %s used!!\n", linecount, id->name);
        exit(0);
    }

    id->nodetype = NODE_REG;
    id->name = NULL;
    id->value = temp2->reg;

    return id;
}

/* Returns the register string */
void getreg(node *root, char reg[])
{
    if (root->value >= R0 && root->value <= R15)
        sprintf(reg, "R%d", root->value - R0);
    else if (root->value >= P0 && root->value <= P3)
        sprintf(reg, "P%d", root->value - P0);
    else if (root->value == BP)
        sprintf(reg, "BP");
    else if (root->value == SP)
        sprintf(reg, "SP");
    else if (root->value == IP)
        sprintf(reg, "IP");
    else if (root->value == PTBR)
        sprintf(reg, "PTBR");
    else if (root->value == PTLR)
        sprintf(reg, "PTLR");
    else if (root->value == EIP)
        sprintf(reg, "EIP");
    else if (root->value == EPN)
        sprintf(reg, "EPN");
    else if (root->value == EC)
        sprintf(reg, "EC");
    else if (root->value == EMA)
        sprintf(reg, "EMA");
}

/* SPL Code Generation */
void codegen(node *root)
{
    char reg1[REG_NAME_MAX_LEN], reg2[REG_NAME_MAX_LEN];
    label *l1, *l2;
    node *temp, *temp2, *temp3;

    if (root == NULL)
        return;

    switch (root->nodetype)
    {
    case NODE_LT:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nLT R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "GT R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "LT R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LT R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_GT:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nGT R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LT R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "GT R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "GT R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_EQ:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nEQ R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "EQ R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "EQ R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "EQ R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_LE:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nLE R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "GE R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "LE R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LE R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_GE:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nGE R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LE R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "GE R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "GE R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_NE:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nNE R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "NE R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "NE R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "NE R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_AND:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nMUL R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MUL R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_OR:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nADD R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "ADD R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_NOT:
        out_linecount++;
        fprintf(fp, "MOV R%d, 1\n", C_REG_BASE + regcount);
        regcount++;
        if (regcount == 5)
        {
            printf("Register Overflow. Please reduce size of your expression.\n");
            exit(0);
        }
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            out_linecount++;
            fprintf(fp, "SUB R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
        }
        else
        {
            codegen(root->ptr1);
            out_linecount++;
            fprintf(fp, "SUB R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
            regcount--;
        }
        break;

    case NODE_STMTLIST:
        codegen(root->ptr1);
        codegen(root->ptr2);
        break;

    case NODE_ADD:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nADD R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nADD R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "ADD R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "ADD R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "ADD R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_SUB:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nSUB R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nSUB R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                out_linecount++;
                fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "SUB R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "SUB R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "SUB R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "SUB R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_MUL:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nMUL R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nMUL R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount - 1, reg1);
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "MUL R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "MUL R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MUL R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_DIV:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nDIV R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nDIV R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                out_linecount++;
                fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "DIV R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "DIV R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "DIV R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "DIV R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_MOD:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nMOD R%d, %s\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, reg2);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount += 2;
                fprintf(fp, "MOV R%d, %s\nMOD R%d, %d\n", C_REG_BASE + regcount, reg1, C_REG_BASE + regcount, root->ptr2->value);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
            }
            else
            {
                out_linecount++;
                fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
                regcount++;
                if (regcount == 5)
                {
                    printf("Register Overflow. Please reduce size of your expression.\n");
                    exit(0);
                }
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MOD R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "MOD R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "MOD R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MOD R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_ASSIGN:
        if (root->ptr1->nodetype == NODE_ADDR_EXPR) //[expr/no]=*
        {

            if (root->ptr1->ptr1->nodetype == NODE_NUM) //[no]=*
            {
                if (root->ptr2->nodetype == NODE_REG) //[no]=reg
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MOV [%d], %s\n", root->ptr1->ptr1->value, reg2);
                }
                else if (root->ptr2->nodetype == NODE_NUM) //[no]=no
                {
                    out_linecount++;
                    fprintf(fp, "MOV [%d], %d\n", root->ptr1->ptr1->value, root->ptr2->value);
                }
                else if (root->ptr2->nodetype == NODE_STRING) //[no]=string
                {
                    out_linecount++;
                    fprintf(fp, "MOV [%d], %s\n", root->ptr1->ptr1->value, root->ptr2->name);
                }
                else if (root->ptr2->nodetype == NODE_PORT) //[no]=port
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "PORT R%d, %s\nMOV [%d], R%d\n", C_REG_BASE + regcount, reg2, root->ptr1->ptr1->value, C_REG_BASE + regcount);
                }
                else //[no]=expr
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MOV [%d], R%d\n", root->ptr1->ptr1->value, C_REG_BASE + regcount - 1);
                    regcount--;
                }
            }
            else if (root->ptr1->ptr1->nodetype == NODE_REG) //[reg]=*
            {
                getreg(root->ptr1->ptr1, reg1);
                if (root->ptr2->nodetype == NODE_REG) //[reg]=reg
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MOV [%s], %s\n", reg1, reg2);
                }
                else if (root->ptr2->nodetype == NODE_NUM) //[reg]=no
                {
                    out_linecount++;
                    fprintf(fp, "MOV [%s], %d\n", reg1, root->ptr2->value);
                }
                else if (root->ptr2->nodetype == NODE_STRING) //[reg]=string
                {
                    out_linecount++;
                    fprintf(fp, "MOV [%s], %s\n", reg1, root->ptr2->name);
                }
                else if (root->ptr2->nodetype == NODE_PORT) //[reg]=port
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "PORT R%d, %s\nMOV [%s], R%d\n", C_REG_BASE + regcount, reg2, reg1, C_REG_BASE + regcount);
                }
                else //[reg]=expr
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MOV [%s], R%d\n", reg1, C_REG_BASE + regcount - 1);
                    regcount--;
                }
            }
            else //[expr]=*
            {
                codegen(root->ptr1->ptr1);
                if (root->ptr2->nodetype == NODE_REG) //[expr]=reg
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "MOV [R%d], %s\n", C_REG_BASE + regcount - 1, reg2);
                }
                else if (root->ptr2->nodetype == NODE_NUM) //[expr]=no
                {
                    out_linecount++;
                    fprintf(fp, "MOV [R%d], %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
                }
                else if (root->ptr2->nodetype == NODE_STRING) //[expr]=string
                {
                    out_linecount++;
                    fprintf(fp, "MOV [R%d], %s\n", C_REG_BASE + regcount - 1, root->ptr2->name);
                }
                else if (root->ptr2->nodetype == NODE_PORT) //[expr]=port
                {
                    getreg(root->ptr2, reg2);
                    out_linecount++;
                    fprintf(fp, "PORT R%d, %s\nMOV [R%d], R%d\n", C_REG_BASE + regcount, reg2, C_REG_BASE + regcount - 1, C_REG_BASE + regcount);
                }
                else //[expr]=expr
                {
                    codegen(root->ptr2);
                    out_linecount++;
                    fprintf(fp, "MOV [R%d], R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                    regcount--;
                }
                regcount--;
            }
        }
        else //reg=*
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG) //reg=reg
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "MOV %s, %s\n", reg1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM) //reg=no
            {
                out_linecount++;
                fprintf(fp, "MOV %s, %d\n", reg1, root->ptr2->value);
            }
            else if (root->ptr2->nodetype == NODE_STRING) //reg=string
            {
                out_linecount++;
                fprintf(fp, "MOV %s, %s\n", reg1, root->ptr2->name);
            }
            else if (root->ptr2->nodetype == NODE_PORT) //reg=port
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "PORT R%d, %s\nMOV %s, R%d\n", C_REG_BASE + regcount, reg2, reg1, C_REG_BASE + regcount);
            }
            else //reg=expr
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "MOV %s, R%d\n", reg1, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        break;

    case NODE_ADDR_EXPR:
        codegen(root->ptr1);
        out_linecount++;
        fprintf(fp, "MOV R%d, [R%d]\n", C_REG_BASE + regcount - 1, C_REG_BASE + regcount - 1);
        break;

    case NODE_NUM:
        out_linecount++;
        fprintf(fp, "MOV R%d, %d\n", C_REG_BASE + regcount, root->value);
        regcount++;
        if (regcount == 5)
        {
            printf("Register Overflow. Please reduce size of your expression.\n");
            exit(0);
        }
        break;

    case NODE_STRING:
        out_linecount++;
        fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, root->name);
        regcount++;
        if (regcount == 5)
        {
            printf("Register Overflow. Please reduce size of your expression.\n");
            exit(0);
        }
        break;

    case NODE_IF:
        l1 = label_create(); // start of else
        l2 = label_create(); // end of else(outside of if else block)
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            out_linecount++;
            fprintf(fp, "JZ %s, %s\n", reg1, label_getName(l1));
        }
        else
        {
            codegen(root->ptr1); // if condition
            out_linecount++;
            fprintf(fp, "JZ R%d, %s\n", C_REG_BASE + regcount - 1, label_getName(l1));
            regcount--;
        }
        codegen(root->ptr2); // if block
        out_linecount++;
        fprintf(fp, "JMP %s\n", label_getName(l2));
        fprintf(fp, "%s:\n", label_getName(l1));
        codegen(root->ptr3); // else block
        fprintf(fp, "%s:\n", label_getName(l2));
        label_free(l1);
        label_free(l2);
        break;

    case NODE_WHILE:
        l1 = label_create(); // start of while
        l2 = label_create(); // end of while
        label_pushWhile(l1, l2);
        fprintf(fp, "%s:\n", label_getName(l1));
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            out_linecount++;
            fprintf(fp, "JZ %s, %s\n", reg1, label_getName(l2));
        }
        else
        {
            codegen(root->ptr1);
            out_linecount++;
            fprintf(fp, "JZ R%d, %s\n", C_REG_BASE + regcount - 1, label_getName(l2));
            regcount--;
        }
        codegen(root->ptr2);
        out_linecount++;
        fprintf(fp, "JMP %s\n", label_getName(l1));
        label_popWhile();
        fprintf(fp, "%s:\n", label_getName(l2));
        label_free(l1);
        label_free(l2);
        break;

    case NODE_BREAK:
        l1 = label_getWhileEnd();
        out_linecount++;
        fprintf(fp, "JMP %s\n", label_getName(l1));
        break;

    case NODE_CONTINUE:
        l1 = label_getWhileStart();
        out_linecount++;
        fprintf(fp, "JMP %s\n", label_getName(l1));
        break;

    case NODE_LOADI:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "LOADI %s, %s\n", reg1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "LOADI %s, %d\n", reg1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LOADI %s, R%d\n", reg1, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "LOADI R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "LOADI R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LOADI R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
            regcount--;
        }
        break;

    case NODE_MULTIPUSH:
        temp = root->ptr1;
        while (temp != NULL)
        {
            getreg(temp, reg1);
            out_linecount++;
            fprintf(fp, "PUSH %s\n", reg1);
            temp = temp->ptr1;
        }
        break;

    case NODE_MULTIPOP:
        temp = root->ptr1;

        //reverse the list
        temp2 = temp->ptr1;
        temp->ptr1 = NULL; // will be the last node in the revesed list
        while (temp2 != NULL)
        {
            temp3 = temp2->ptr1; // nextNode
            temp2->ptr1 = temp;  // currNode->next=prevNode
            temp = temp2;        // prevNode=currNode
            temp2 = temp3;       // currNode=nextNode
        }

        while (temp != NULL)
        {
            getreg(temp, reg1);
            out_linecount++;
            fprintf(fp, "POP %s\n", reg1);
            temp = temp->ptr1;
        }

        break;

    case NODE_LOAD:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "LOAD %s, %s\n", reg1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "LOAD %s, %d\n", reg1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LOAD %s, R%d\n", reg1, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "LOAD R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "LOAD R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "LOAD R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
            regcount--;
        }
        break;

    case NODE_STORE:
        if (root->ptr1->nodetype == NODE_REG)
        {
            getreg(root->ptr1, reg1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "STORE %s, %s\n", reg1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "STORE %s, %d\n", reg1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "STORE %s, R%d\n", reg1, C_REG_BASE + regcount - 1);
                regcount--;
            }
        }
        else
        {
            codegen(root->ptr1);
            if (root->ptr2->nodetype == NODE_REG)
            {
                getreg(root->ptr2, reg2);
                out_linecount++;
                fprintf(fp, "STORE R%d, %s\n", C_REG_BASE + regcount - 1, reg2);
            }
            else if (root->ptr2->nodetype == NODE_NUM)
            {
                out_linecount++;
                fprintf(fp, "STORE R%d, %d\n", C_REG_BASE + regcount - 1, root->ptr2->value);
            }
            else
            {
                codegen(root->ptr2);
                out_linecount++;
                fprintf(fp, "STORE R%d, R%d\n", C_REG_BASE + regcount - 2, C_REG_BASE + regcount - 1);
                regcount--;
            }
            regcount--;
        }
        break;

    case NODE_BACKUP:
        out_linecount++;
        fprintf(fp, "BACKUP\n");
        break;

    case NODE_RESTORE:
        out_linecount++;
        fprintf(fp, "RESTORE\n");
        break;

    case NODE_RETURN:
        out_linecount++;
        fprintf(fp, "RET\n");
        break;

    case NODE_IRETURN:
        out_linecount++;
        fprintf(fp, "IRET\n");
        break;

    case NODE_REG:
        getreg(root, reg1);
        out_linecount++;
        fprintf(fp, "MOV R%d, %s\n", C_REG_BASE + regcount, reg1);
        regcount++;
        if (regcount == 5)
        {
            printf("Register Overflow. Please reduce size of your expression.\n");
            exit(0);
        }
        break;

    case NODE_HALT:
        out_linecount++;
        fprintf(fp, "HALT\n");
        break;

    case NODE_BREAKPOINT:
        out_linecount++;
        fprintf(fp, "BRKP\n");
        break;

    case NODE_READ:
        out_linecount++;
        fprintf(fp, "IN\n");
        break;

    case NODE_READI:
        getreg(root->ptr1, reg1);
        out_linecount += 2;
        fprintf(fp, "INI\nPORT %s, P0\n", reg1);
        break;

    case NODE_PRINT:
        codegen(root->ptr1);
        out_linecount += 2;
        fprintf(fp, "PORT P1, R%d\nOUT\n", C_REG_BASE + regcount - 1);
        regcount--;
        break;

    case NODE_INLINE:
        out_linecount++;
        fprintf(fp, "%s\n", root->ptr1->name);
        break;

    case NODE_ENCRYPT:
        getreg(root->ptr1, reg1);
        out_linecount++;
        fprintf(fp, "ENCRYPT %s\n", reg1);
        break;

    case NODE_LABEL_DEF:
        fprintf(fp, "%s:\n", root->ptr1->name);
        break;

    case NODE_CALL:
        if (root->ptr1->nodetype == NODE_NUM)
        {
            fprintf(fp, "CALL %d\n", root->ptr1->value);
        }
        else
        {
            if (label_get(root->ptr1->name) == NULL)
            {
                fprintf(stderr, "%d: Label '%s' is not declared", root->value, root->ptr1->name);
                exit(0);
            }
            else
            {
                fprintf(fp, "CALL %s\n", root->ptr1->name);
            }
        }
        break;

    case NODE_GOTO:
        if (root->ptr1->nodetype == NODE_NUM)
        {
            fprintf(fp, "JMP %d\n", root->ptr1->value);
        }
        else
        {
            if (label_get(root->ptr1->name) == NULL)
            {
                fprintf(stderr, "%d: Label '%s' is not declared", root->value, root->ptr1->name);
            }
            else
            {
                fprintf(fp, "JMP %s\n", root->ptr1->name);
            }
        }
        break;

    default:
        fprintf(stderr, "Unknown Command %d %s\n", root->nodetype, root->name);
        return;
    }
}

int main(int argc, char **argv)
{
    char filename[FILENAME_MAX_LEN], op_name[FILENAME_MAX_LEN];
    int ch;
    FILE *input_fp;

    strcpy(filename, argv[1]);
    file_expandPath(filename);

    input_fp = fopen(filename, "r");
    if (!input_fp)
    {
        printf("Invalid input file\n");
        return 0;
    }

    yyin = input_fp;
    file_getOpFileName(op_name, filename);

    // Write the output to a temp file
    fp = fopen(".temp", "w");
    out_linecount++;
    yyparse();
    fclose(input_fp);
    fclose(fp);

    // Compile was success now we actually open the result file and write
    input_fp = fopen(".temp", "r");
    if (!input_fp)
    {
        printf("Writing compiled code to file failed!!\n");
        return 0;
    }

    fp = fopen(op_name, "w");
    if (!fp)
    {
        fclose(input_fp);
        printf("Writing compiled code to file failed!!\n");
        return 0;
    }

    while ((ch = fgetc(input_fp)) != EOF)
        fputc(ch, fp);

    fclose(input_fp);
    fclose(fp);

    return 0;
}
