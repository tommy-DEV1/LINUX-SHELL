#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "shell.h"
#include "node.h"
#include "parser.h"

struct nodeStruct *newNode(enum nodeTypeE type)
{
    struct nodeStruct *node = malloc(sizeof(struct nodeStruct));

    if (!node)
    {
        return NULL;
    }

    memset(node, 0, sizeof(struct nodeStruct));
    node->type = type;

    return node;
}

void addChildNode(struct nodeStruct *parent, struct nodeStruct *child)
{
    if (!parent || !child)
    {
        return;
    }

    if (!parent->firstChild)
    {
        parent->firstChild = child;
    }
    else
    {
        struct nodeStruct *sibling = parent->firstChild;

        while (sibling->nextSibling)
        {
            sibling = sibling->nextSibling;
        }

        sibling->nextSibling = child;
        child->previousSibling = sibling;
    }
    parent->children++;
}

void setNodeValueString(struct nodeStruct *node, char *val)
{
    node->valueType = VAL_STR;

    if (!val)
    {
        node->val.str = NULL;
    }
    else
    {
        char *val2 = malloc(strlen(val) + 1);

        if (!val2)
        {
            node->val.str = NULL;
        }
        else
        {
            strcpy(val2, val);
            node->val.str = val2;
        }
    }
}

void freeNodeTree(struct nodeStruct *node)
{
    if (!node)
    {
        return;
    }

    struct nodeStruct *child = node->firstChild;

    while (child)
    {
        struct nodeStruct *next = child->nextSibling;
        freeNodeTree(child);
        child = next;
    }

    if (node->valueType == VAL_STR)
    {
        if (node->val.str)
        {
            free(node->val.str);
        }
    }
    free(node);
}
