#include <string.h>
#include "shell.h"
#include "symtab/symtab.h"

extern char **environ;

void initsh()
{
    initializeSymtab();

    struct symtabEntryStruct *entry;
    char **p2 = environ;

    while (*p2)
    {
        char *eq = strchr(*p2, '=');
        if (eq)
        {
            int len = eq - (*p2);
            char name[len + 1];

            strncpy(name, *p2, len);
            name[len] = '\0';
            entry = addToSymtab(name);

            if (entry)
            {
                symtabEntrySetValue(entry, eq + 1);
                entry->flags |= FLAG_EXPORT;
            }
        }
        else
        {
            entry = addToSymtab(*p2);
        }
        p2++;
    }

    entry = addToSymtab("PS1");
    symtabEntrySetValue(entry, "$ ");

    entry = addToSymtab("PS2");
    symtabEntrySetValue(entry, "> ");
}
