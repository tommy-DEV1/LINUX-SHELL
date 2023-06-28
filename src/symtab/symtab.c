#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../shell.h"
#include "../node.h"
#include "../parser.h"
#include "symtab.h"

struct symtabStackStruct symtabStack;
int symtabLevel;

void initializeSymtab(void)
{
    symtabStack.symtabCount = 1;
    symtabLevel = 0;

    struct symtabStruct *globalSymtab = malloc(sizeof(struct symtabStruct));

    if (!globalSymtab)
    {
        fprintf(stderr, "fatal error: no memory for global symbol table\n");
        exit(EXIT_FAILURE);
    }

    memset(globalSymtab, 0, sizeof(struct symtabStruct));
    symtabStack.globalSymtab = globalSymtab;
    symtabStack.localSymtab = globalSymtab;
    symtabStack.symtabList[0] = globalSymtab;
    globalSymtab->level = 0;
}

struct symtabStruct *newSymtab(int level)
{
    struct symtabStruct *symtab = malloc(sizeof(struct symtabStruct));

    if (!symtab)
    {
        fprintf(stderr, "fatal error: no memory for new symbol table\n");
        exit(EXIT_FAILURE);
    }

    memset(symtab, 0, sizeof(struct symtabStruct));
    symtab->level = level;
    return symtab;
}

void freeSymtab(struct symtabStruct *symtab)
{
    if (symtab == NULL)
    {
        return;
    }

    struct symtabEntryStruct *entry = symtab->first;

    while (entry)
    {
        if (entry->name)
        {
            free(entry->name);
        }

        if (entry->val)
        {
            free(entry->val);
        }

        if (entry->functionBody)
        {
            freeNodeTree(entry->functionBody);
        }

        struct symtabEntryStruct *next = entry->next;
        free(entry);
        entry = next;
    }

    free(symtab);
}

void dumpLocalSymtab(void)
{
    struct symtabStruct *symtab = symtabStack.localSymtab;
    int i = 0;
    int indent = symtab->level * 4;

    fprintf(stderr, "%*sSymbol table [Level %d]:\r\n", indent, " ", symtab->level);
    fprintf(stderr, "%*s===========================\r\n", indent, " ");
    fprintf(stderr, "%*s  No               Symbol                    Val\r\n", indent, " ");
    fprintf(stderr, "%*s------ -------------------------------- ------------\r\n", indent, " ");

    struct symtabEntryStruct *entry = symtab->first;

    while (entry)
    {
        fprintf(stderr, "%*s[%04d] %-32s '%s'\r\n", indent, " ",
                i++, entry->name, entry->val);
        entry = entry->next;
    }

    fprintf(stderr, "%*s------ -------------------------------- ------------\r\n", indent, " ");
}

struct symtabEntryStruct *addToSymtab(char *symbol)
{
    if (!symbol || symbol[0] == '\0')
    {
        return NULL;
    }

    struct symtabStruct *st = symtabStack.localSymtab;
    struct symtabEntryStruct *entry = NULL;

    if ((entry = doLookup(symbol, st)))
    {
        return entry;
    }

    entry = malloc(sizeof(struct symtabEntryStruct));

    if (!entry)
    {
        fprintf(stderr, "fatal error: no memory for new symbol table entry\n");
        exit(EXIT_FAILURE);
    }

    memset(entry, 0, sizeof(struct symtabEntryStruct));
    entry->name = malloc(strlen(symbol) + 1);

    if (!entry->name)
    {
        fprintf(stderr, "fatal error: no memory for new symbol table entry\n");
        exit(EXIT_FAILURE);
    }

    strcpy(entry->name, symbol);

    if (!st->first)
    {
        st->first = entry;
        st->last = entry;
    }
    else
    {
        st->last->next = entry;
        st->last = entry;
    }

    return entry;
}

struct symtabEntryStruct *doLookup(char *str, struct symtabStruct *symtable)
{
    if (!str || !symtable)
    {
        return NULL;
    }

    struct symtabEntryStruct *entry = symtable->first;

    while (entry)
    {
        if (strcmp(entry->name, str) == 0)
        {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

struct symtabEntryStruct *getSymtabEntry(char *str)
{
    int i = symtabStack.symtabCount - 1;

    do
    {
        struct symtabStruct *symtab = symtabStack.symtabList[i];
        struct symtabEntryStruct *entry = doLookup(str, symtab);

        if (entry)
        {
            return entry;
        }

    } while (--i >= 0);

    return NULL;
}

void symtabEntrySetValue(struct symtabEntryStruct *entry, char *val)
{
    if (entry->val)
    {
        free(entry->val);
    }

    if (!val)
    {
        entry->val = NULL;
    }
    else
    {
        char *val2 = malloc(strlen(val) + 1);

        if (val2)
        {
            strcpy(val2, val);
        }
        else
        {
            fprintf(stderr, "error: no memory for symbol table entry's value\n");
        }

        entry->val = val2;
    }
}

int rememberFromSymtab(struct symtabEntryStruct *entry, struct symtabStruct *symtab)
{
    int res = 0;
    if (entry->val)
    {
        free(entry->val);
    }

    if (entry->functionBody)
    {
        freeNodeTree(entry->functionBody);
    }

    free(entry->name);

    if (symtab->first == entry)
    {
        symtab->first = symtab->first->next;

        if (symtab->last == entry)
        {
            symtab->last = NULL;
        }
        res = 1;
    }
    else
    {
        struct symtabEntryStruct *e = symtab->first;
        struct symtabEntryStruct *p = NULL;

        while (e && e != entry)
        {
            p = e;
            e = e->next;
        }

        if (e == entry)
        {
            p->next = entry->next;
            res = 1;
        }
    }

    free(entry);
    return res;
}

void symtabStackAdd(struct symtabStruct *symtab)
{
    symtabStack.symtabList[symtabStack.symtabCount++] = symtab;
    symtabStack.localSymtab = symtab;
}

struct symtabStruct *symtabStackPush(void)
{
    struct symtabStruct *st = newSymtab(++symtabLevel);
    symtabStackAdd(st);
    return st;
}

struct symtabStruct *symtabStackPop(void)
{
    if (symtabStack.symtabCount == 0)
    {
        return NULL;
    }

    struct symtabStruct *st = symtabStack.symtabList[symtabStack.symtabCount - 1];

    symtabStack.symtabList[--symtabStack.symtabCount] = NULL;
    symtabLevel--;

    if (symtabStack.symtabCount == 0)
    {
        symtabStack.localSymtab = NULL;
        symtabStack.globalSymtab = NULL;
    }
    else
    {
        symtabStack.localSymtab = symtabStack.symtabList[symtabStack.symtabCount - 1];
    }

    return st;
}

struct symtabStruct *getLocalSymtab(void)
{
    return symtabStack.localSymtab;
}

struct symtabStruct *getGlobalSymtab(void)
{
    return symtabStack.globalSymtab;
}

struct symtabStackStruct *getSymtabStack(void)
{
    return &symtabStack;
}
