#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "shell.h"
#include "scanner.h"
#include "source.h"

char *tokenBuffer = NULL;
int tokenBufferSize = 0;
int tokenBufferIndex = -1;

/* special token to indicate end of input */
struct tokenStruct EOFToken =
    {
        .textLength = 0,
};

void addToBuffer(char c)
{
    tokenBuffer[tokenBufferIndex++] = c;

    if (tokenBufferIndex >= tokenBufferSize)
    {
        char *tmp = realloc(tokenBuffer, tokenBufferSize * 2);

        if (!tmp)
        {
            errno = ENOMEM;
            return;
        }

        tokenBuffer = tmp;
        tokenBufferSize *= 2;
    }
}

struct tokenStruct *createToken(char *str)
{
    struct tokenStruct *tok = malloc(sizeof(struct tokenStruct));

    if (!tok)
    {
        return NULL;
    }

    memset(tok, 0, sizeof(struct tokenStruct));
    tok->textLength = strlen(str);

    char *nstr = malloc(tok->textLength + 1);

    if (!nstr)
    {
        free(tok);
        return NULL;
    }

    strcpy(nstr, str);
    tok->text = nstr;

    return tok;
}

void freeToken(struct tokenStruct *tok)
{
    if (tok->text)
    {
        free(tok->text);
    }
    free(tok);
}

struct tokenStruct *tokenize(struct sourceStruct *src)
{
    int endloop = 0;

    if (!src || !src->buffer || !src->bufsize)
    {
        errno = ENODATA;
        return &EOFToken;
    }

    if (!tokenBuffer)
    {
        tokenBufferSize = 1024;
        tokenBuffer = malloc(tokenBufferSize);
        if (!tokenBuffer)
        {
            errno = ENOMEM;
            return &EOFToken;
        }
    }

    tokenBufferIndex = 0;
    tokenBuffer[0] = '\0';

    char nc = nextCharacter(src);
    char nc2;
    int i;

    if (nc == ERRCHAR || nc == EOF)
    {
        return &EOFToken;
    }

    do
    {
        switch (nc)
        {
        case '"':
        case '\'':
        case '`':
            /*
             * for quote chars, add the quote, as well as everything between this
             * quote and the matching closing quote, to the token buffer.
             */
            addToBuffer(nc);
            i = findClosingQuote(src->buffer + src->curpos);

            if (!i)
            {
                /* failed to find matching quote. return error token */
                src->curpos = src->bufsize;
                fprintf(stderr, "error: missing closing quote '%c'\n", nc);
                return &EOFToken;
            }

            while (i--)
            {
                addToBuffer(nextCharacter(src));
            }
            break;

        case '\\':
            /* get the next char after the backslah */
            nc2 = nextCharacter(src);

            /*
             * discard backslash+newline '\\n' combination.. in an interactive shell, this
             * case shouldn't happen as the readCommand() function discards the '\\n' sequence
             * automatically.. however, if the input comes from a command string or script,
             * we might encounter this sequence.
             */
            if (nc2 == '\n')
            {
                break;
            }

            /* add the backslah to the token buffer */
            addToBuffer(nc);

            /* add the escaped char to the token buffer */
            if (nc2 > 0)
            {
                addToBuffer(nc2);
            }
            break;

        case '$':
            /* add the '$' to buffer and check the char after it */
            addToBuffer(nc);
            nc = peekCharacter(src);

            /* we have a '${' or '$(' sequence */
            if (nc == '{' || nc == '(')
            {
                /* find the matching closing brace */
                i = findClosingBrace(src->buffer + src->curpos + 1);

                if (!i)
                {
                    /* failed to find matching brace. return error token */
                    src->curpos = src->bufsize;
                    fprintf(stderr, "error: missing closing brace '%c'\n", nc);
                    return &EOFToken;
                }

                while (i--)
                {
                    addToBuffer(nextCharacter(src));
                }
            }
            /*
             * we have a special parameter name, such as $0, $*, $@, $#,
             * or a positional parameter name, such as $1, $2, ...
             */
            else if (isalnum(nc) || nc == '*' || nc == '@' || nc == '#' ||
                     nc == '!' || nc == '?' || nc == '$')
            {
                addToBuffer(nextCharacter(src));
            }
            break;

        case ' ':
        case '\t':
            if (tokenBufferIndex > 0)
            {
                endloop = 1;
            }
            break;

        case '\n':
            if (tokenBufferIndex > 0)
            {
                unget_char(src);
            }
            else
            {
                addToBuffer(nc);
            }
            endloop = 1;
            break;

        default:
            addToBuffer(nc);
            break;
        }

        if (endloop)
        {
            break;
        }

    } while ((nc = nextCharacter(src)) != EOF);

    if (tokenBufferIndex == 0)
    {
        return &EOFToken;
    }

    if (tokenBufferIndex >= tokenBufferSize)
    {
        tokenBufferIndex--;
    }
    tokenBuffer[tokenBufferIndex] = '\0';

    struct tokenStruct *tok = createToken(tokenBuffer);
    if (!tok)
    {
        fprintf(stderr, "error: failed to alloc buffer: %s\n", strerror(errno));
        return &EOFToken;
    }

    tok->src = src;
    return tok;
}
