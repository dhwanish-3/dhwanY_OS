/*
Interface that handles SPL labels.
*/

#include "label.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern int linecount;

static label *_root_label = NULL;
static int _namegen = 1;
static l_while *_rootWhile = NULL;

/* Create a new label with a random unused name */
label *label_create()
{
    char name[30];
    sprintf(name, "_L%d", _namegen);
    _namegen++;

    return _label_create(name);
}

/* Create a new label with given name */
label *_label_create(const char *name)
{
    label *new_label = (label *)malloc(sizeof(label));
    new_label->name = strdup(name);
    new_label->next = NULL;

    return new_label;
}

/* Adds a new label to the labels list */
label *label_add(const char *name)
{
    label *new_label;

    if (label_get(name) != NULL)
    {
        fprintf(stderr, "\n%d: Label '%s' redeclared.", linecount, name);
        exit(0);
    }

    new_label = _label_create(name);
    new_label->next = _root_label;
    _root_label = new_label;

    return new_label;
}

/* Get the label element with the name */
label *label_get(const char *name)
{
    label *temp = _root_label;
    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

/* Returns the name of the label */
char *label_getName(label *ll)
{
    return ll->name;
}

/* Deallocates the label */
void label_free(label *ll)
{
    free(ll->name);
    free(ll);
}

/* Push the label of a while block into the stack */
void label_pushWhile(label *start, label *end)
{
    l_while *newl = (l_while *)malloc(sizeof(l_while));
    newl->start = start;
    newl->end = end;
    newl->next = _rootWhile;
    _rootWhile = newl;
}

/* Pop the label of a while block from the stack */
void label_popWhile()
{
    l_while *temp = _rootWhile;
    _rootWhile = _rootWhile->next;
    free(temp);
}

/* Get the end label of the innermost while */
label *label_getWhileEnd()
{
    return _rootWhile->end;
}

/* Get the start label of the innermost while */
label *label_getWhileStart()
{
    return _rootWhile->start;
}