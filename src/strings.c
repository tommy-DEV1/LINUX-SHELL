#include <stdlib.h>
#include <string.h>
#include "shell.h"

/*
 * search string for any one of the passed characters.
 *
 * returns a char pointer to the first occurence of any of the characters,
 * NULL if none found.
 */
char *stringCharacterAny(char *string, char *chars)
{
    if (!string || !chars)
    {
        return NULL;
    }
    char *s = string;
    while (*s)
    {
        char *c = chars;
        while (*c)
        {
            if (*s == *c)
            {
                return s;
            }
            c++;
        }
        s++;
    }
    return NULL;
}

/*
 * return the passed string value, quoted in a format that can
 * be used for reinput to the shell.
 */
char *quoteValue(char *val, int addQuotes)
{
    char *res = NULL;
    size_t len;
    /* empty string */
    if (!val || !*val)
    {
        len = addQuotes ? 3 : 1;
        res = malloc(len);
        if (!res)
        {
            return NULL;
        }
        strcpy(res, addQuotes ? "\"\"" : "");
        return res;
    }
    /* count the number of quotes needed */
    len = 0;
    char *v = val, *p;
    while (*v)
    {
        switch (*v)
        {
        case '\\':
        case '`':
        case '$':
        case '"':
            len++;
            break;
        }
        v++;
    }
    len += strlen(val);
    /* add two for the opening and closing quotes (optional) */
    if (addQuotes)
    {
        len += 2;
    }
    /* alloc memory for quoted string */
    res = malloc(len + 1);
    if (!res)
    {
        return NULL;
    }
    p = res;
    /* add opening quote (optional) */
    if (addQuotes)
    {
        *p++ = '"';
    }
    /* copy quoted val */
    v = val;
    while (*v)
    {
        switch (*v)
        {
        case '\\':
        case '`':
        case '$':
        case '"':
            /* add '\' for quoting */
            *p++ = '\\';
            /* copy char */
            *p++ = *v++;
            break;

        default:
            /* copy next char */
            *p++ = *v++;
            break;
        }
    }
    /* add closing quote (optional) */
    if (addQuotes)
    {
        *p++ = '"';
    }
    *p = '\0';
    return res;
}

/*
 * alloc memory for, or extend the host (or user) names buffer if needed..
 * in the first call, the buffer is initialized to 32 entries.. subsequent
 * calls result in the buffer size doubling, so that it becomes 64, 128, ...
 * count is the number of used entries in the buffer, while len is the number
 * of alloc'd entries (size of buffer divided by sizeof(char **)).
 *
 * returns 1 if the buffer is alloc'd/extended, 0 otherwise.
 */
int checkBufferBounds(int *count, int *len, char ***buf)
{
    if (*count >= *len)
    {
        if (!(*buf))
        {
            /* first call. alloc memory for the buffer */
            *buf = malloc(32 * sizeof(char **));
            if (!(*buf))
            {
                return 0;
            }
            *len = 32;
        }
        else
        {
            /* subsequent calls. extend the buffer */
            int newlen = (*len) * 2;
            char **hn2 = realloc(*buf, newlen * sizeof(char **));
            if (!hn2)
            {
                return 0;
            }
            *buf = hn2;
            *len = newlen;
        }
    }
    return 1;
}

/*
 * free the memory used to store the strings list pointed to by buf.
 */
void freeBuffer(int len, char **buf)
{
    if (!len)
    {
        return;
    }

    while (len--)
    {
        free(buf[len]);
    }
    free(buf);
}
