#include <stdio.h>
#include "shell.h"
#include "symtab/symtab.h"

void printPrompt1(void)
{
    struct symtabEntryStruct *entry = getSymtabEntry("PS1");

    if (entry && entry->val)
    {
        fprintf(stderr, "%s", entry->val);
    }
    else
    {
        fprintf(stderr, "$ ");
    }
}

void printPrompt2(void)
{
    struct symtabEntryStruct *entry = getSymtabEntry("PS2");

    if (entry && entry->val)
    {
        fprintf(stderr, "%s", entry->val);
    }
    else
    {
        fprintf(stderr, "> ");
    }
}
