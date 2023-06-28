#ifndef SCANNER_H
#define SCANNER_H

struct tokenStruct
{
    struct sourceStruct *src; /* source of input */
    int textLength;           /* length of token text */
    char *text;               /* token text */
};

/* the special EOF token, which indicates the end of input */
extern struct tokenStruct EOFToken;

struct tokenStruct *tokenize(struct sourceStruct *src);
void freeToken(struct tokenStruct *tok);

#endif
