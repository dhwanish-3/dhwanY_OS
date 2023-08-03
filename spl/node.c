/*
Interface that handles SPL nodes.
*/

#include "node.h"

#include <stdlib.h>
#include <string.h>

/* Creates a terminal node */
node *create_termNode(char nodetype, char *name, int value)
{
    node *tnode = malloc(sizeof(node));

    if (name != NULL)
        tnode->name = strdup(name);

    tnode->nodetype = nodetype;
    tnode->value = value;
    tnode->ptr1 = NULL;
    tnode->ptr2 = NULL;
    tnode->ptr3 = NULL;

    return tnode;
}

/* Creates a non-terminal node */
node *create_nontermNode(char nodetype, node *a, node *b)
{
    node *temp = malloc(sizeof(node));

    temp->nodetype = nodetype;
    temp->name = NULL;
    temp->ptr1 = a;
    temp->ptr2 = b;
    temp->ptr3 = NULL;

    return temp;
}

/* Retrieves the node type */
char node_getType(node *nn)
{
    return nn->nodetype;
}

/* Retrieves the node name */
char *node_getName(node *nn)
{
    return nn->name;
}

/* Assigns the tree pointers */
node *create_tree(node *a, node *b, node *c, node *d)
{
    a->ptr1 = b;
    a->ptr2 = c;
    a->ptr3 = d;

    return a;
}