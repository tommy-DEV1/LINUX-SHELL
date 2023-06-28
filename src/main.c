#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "shell.h"
#include "source.h"
#include "parser.h"
#include "executor.h"

int main(int argc, char **argv)
{
    char *cmd;

    initsh();

    do
    {
        printPrompt1();
        cmd = readCommand();
        if (!cmd)
        {
            exit(EXIT_SUCCESS);
        }
        if (cmd[0] == '\0' || strcmp(cmd, "\n") == 0)
        {
            free(cmd);
            continue;
        }
        if (strcmp(cmd, "exit\n") == 0)
        {
            free(cmd);
            break;
        }
        struct sourceStruct src;
        src.buffer = cmd;
        src.bufsize = strlen(cmd);
        src.curpos = INIT_SRC_POS;
        parseAndExecute(&src);
        free(cmd);
    } while (1);
    exit(EXIT_SUCCESS);
}

char *readCommand(void)
{
    char buf[1024];
    char *ptr = NULL;
    char ptrlen = 0;
    while (fgets(buf, 1024, stdin))
    {
        int buflen = strlen(buf);
        if (!ptr)
        {
            ptr = malloc(buflen + 1);
        }
        else
        {
            char *ptr2 = realloc(ptr, ptrlen + buflen + 1);
            if (ptr2)
            {
                ptr = ptr2;
            }
            else
            {
                free(ptr);
                ptr = NULL;
            }
        }
        if (!ptr)
        {
            fprintf(stderr, "error: failed to alloc buffer: %s\n", strerror(errno));
            return NULL;
        }
        strcpy(ptr + ptrlen, buf);
        if (buf[buflen - 1] == '\n')
        {
            if (buflen == 1 || buf[buflen - 2] != '\\')
            {
                return ptr;
            }
            ptr[ptrlen + buflen - 2] = '\0';
            buflen -= 2;
            printPrompt2();
        }
        ptrlen += buflen;
    }
    return ptr;
}

int parseAndExecute(struct sourceStruct *src)
{
    skipWhiteSpaces(src);

    struct tokenStruct *tok = tokenize(src);

    if (tok == &EOFToken)
    {
        return 0;
    }

    while (tok && tok != &EOFToken)
    {
        struct nodeStruct *cmd = parseSimpleCommand(tok);

        if (!cmd)
        {
            break;
        }

        doSimpleCommand(cmd);
        freeNodeTree(cmd);
        tok = tokenize(src);
    }
    return 1;
}
