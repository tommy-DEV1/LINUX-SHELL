#ifndef SYMTAB_H
#define SYMTAB_H

#include "../node.h"

/* the type of a symbol table entry's value */
enum symbolTypeE
{
    SYM_STR,
    SYM_FUNC,
};

/* the symbol table entry structure */
struct symtabEntryStruct
{
    char *name;                      /* key */
    enum symbolTypeE valueType;       /* type of value */
    char *val;                       /* value */
    unsigned int flags;              /* flags like readonly, export, ... */
    struct symtabEntryStruct *next;  /* pointer to the next entry */
    struct nodeStruct *functionBody; /* func's body AST (for funcs) */
};

struct symtabStruct
{
    int level;
    struct symtabEntryStruct *first, *last;
};

/* values for the flags field of struct symtabEntryStruct */
#define FLAG_EXPORT (1 << 0) /* export entry to forked commands */

/* the symbol table stack structure */
#define MAX_SYMTAB 256 /* maximum allowed symbol tables in the stack */

struct symtabStackStruct
{
    int symtabCount;                             /* number of tables in the stack */
    struct symtabStruct *symtabList[MAX_SYMTAB]; /* pointers to the tables */
    struct symtabStruct *globalSymtab,
        *localSymtab; /*
                       * pointers to the local
                       * and global symbol tables
                       */
};

struct symtabStruct *newSymtab(int level);
struct symtabStruct *symtabStackPush(void);
struct symtabStruct *symtabStackPop(void);
int rememberFromSymtab(struct symtabEntryStruct *entry, struct symtabStruct *symtab);
struct symtabEntryStruct *addToSymtab(char *symbol);
struct symtabEntryStruct *doLookup(char *str, struct symtabStruct *symtable);
struct symtabEntryStruct *getSymtabEntry(char *str);
struct symtabStruct *getLocalSymtab(void);
struct symtabStruct *getGlobalSymtab(void);
struct symtabStackStruct *getSymtabStack(void);
void initializeSymtab(void);
void dumpLocalSymtab(void);
void freeSymtab(struct symtabStruct *symtab);
void symtabEntrySetValue(struct symtabEntryStruct *entry, char *val);

#endif
