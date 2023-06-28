#include <errno.h>
#include "shell.h"
#include "source.h"

void unget_char(struct sourceStruct *src)
{
    if (src->curpos < 0)
    {
        return;
    }

    src->curpos--;
}

char nextCharacter(struct sourceStruct *src)
{
    if (!src || !src->buffer)
    {
        errno = ENODATA;
        return ERRCHAR;
    }

    if (src->curpos == INIT_SRC_POS)
    {
        src->curpos = -1;
    }

    if (++src->curpos >= src->bufsize)
    {
        src->curpos = src->bufsize;
        return EOF;
    }

    return src->buffer[src->curpos];
}

char peekCharacter(struct sourceStruct *src)
{
    if (!src || !src->buffer)
    {
        errno = ENODATA;
        return ERRCHAR;
    }

    long pos = src->curpos;

    if (pos == INIT_SRC_POS)
    {
        pos++;
    }
    pos++;

    if (pos >= src->bufsize)
    {
        return EOF;
    }

    return src->buffer[pos];
}

void skipWhiteSpaces(struct sourceStruct *src)
{
    char c;

    if (!src || !src->buffer)
    {
        return;
    }

    while (((c = peekCharacter(src)) != EOF) && (c == ' ' || c == '\t'))
    {
        nextCharacter(src);
    }
}
