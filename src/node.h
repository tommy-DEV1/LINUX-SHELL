#ifndef NODE_H
#define NODE_H

enum nodeTypeE
{
    NODE_COMMAND, /* simple command */
    NODE_VAR,     /* variable name (or simply, a word) */
};

enum valueTypeE
{
    VAL_SINT = 1, /* signed int */
    VAL_UINT,     /* unsigned int */
    VAL_SLLONG,   /* signed long long */
    VAL_ULLONG,   /* unsigned long long */
    VAL_FLOAT,    /* floating point */
    VAL_LDOUBLE,  /* long double */
    VAL_CHR,      /* char */
    VAL_STR,      /* str (char pointer) */
};

union symtabValueUnion
{
    long sint;
    unsigned long uint;
    long long sllong;
    unsigned long long ullong;
    double sfloat;
    long double ldouble;
    char chr;
    char *str;
};

struct nodeStruct
{
    enum nodeTypeE type;                              /* type of this node */
    enum valueTypeE valueType;                        /* type of this node's val field */
    union symtabValueUnion val;                       /* value of this node */
    int children;                                     /* number of child nodes */
    struct nodeStruct *firstChild;                    /* first child node */
    struct nodeStruct *nextSibling, *previousSibling; /*
                                                       * if this is a child node, keep
                                                       * pointers to prev/next siblings
                                                       */
};

struct nodeStruct *newNode(enum nodeTypeE type);
void addChildNode(struct nodeStruct *parent, struct nodeStruct *child);
void freeNodeTree(struct nodeStruct *node);
void setNodeValueString(struct nodeStruct *node, char *val);

#endif
