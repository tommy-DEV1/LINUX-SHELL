#ifndef SOURCE_H
#define SOURCE_H

#define EOF (-1)
#define ERRCHAR (0)

#define INIT_SRC_POS (-2)

struct sourceStruct
{
    char *buffer; /* the input text */
    long bufsize; /* size of the input text */
    long curpos;  /* absolute char position in source */
};

char nextCharacter(struct sourceStruct *src);
void unget_char(struct sourceStruct *src);
char peekCharacter(struct sourceStruct *src);
void skipWhiteSpaces(struct sourceStruct *src);

#endif
