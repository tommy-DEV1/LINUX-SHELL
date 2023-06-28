#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "shell.h"
#include "node.h"
#include "executor.h"

char *searchPath(char *file)
{
    char *PATH = getenv("PATH");
    char *p = PATH;
    char *p2;

    while (p && *p)
    {
        p2 = p;

        while (*p2 && *p2 != ':')
        {
            p2++;
        }

        int plen = p2 - p;
        if (!plen)
        {
            plen = 1;
        }

        int alen = strlen(file);
        char path[plen + 1 + alen + 1];

        strncpy(path, p, p2 - p);
        path[p2 - p] = '\0';

        if (p2[-1] != '/')
        {
            strcat(path, "/");
        }

        strcat(path, file);

        struct stat st;
        if (stat(path, &st) == 0)
        {
            if (!S_ISREG(st.st_mode))
            {
                errno = ENOENT;
                p = p2;
                if (*p2 == ':')
                {
                    p++;
                }
                continue;
            }

            p = malloc(strlen(path) + 1);
            if (!p)
            {
                return NULL;
            }

            strcpy(p, path);
            return p;
        }
        else /* file not found */
        {
            p = p2;
            if (*p2 == ':')
            {
                p++;
            }
        }
    }

    errno = ENOENT;
    return NULL;
}

int doExecutionCommand(int argc, char **argv)
{
    if (strchr(argv[0], '/'))
    {
        execv(argv[0], argv);
    }
    else
    {
        char *path = searchPath(argv[0]);
        if (!path)
        {
            return 0;
        }
        execv(path, argv);
        free(path);
    }
    return 0;
}

static inline void freeArgv(int argc, char **argv)
{
    if (!argc)
    {
        return;
    }

    while (argc--)
    {
        free(argv[argc]);
    }
}

int doSimpleCommand(struct nodeStruct *node)
{
    if (!node)
    {
        return 0;
    }

    struct nodeStruct *child = node->firstChild;
    if (!child)
    {
        return 0;
    }

    int argc = 0;  /* arguments count */
    int targc = 0; /* total alloc'd arguments count */
    char **argv = NULL;
    char *str;

    while (child)
    {
        str = child->val.str;
        /*perform word expansion */
        struct wordStruct *w = wordExpand(str);

        /* word expansion failed */
        if (!w)
        {
            child = child->nextSibling;
            continue;
        }

        /* add the words to the arguments list */
        struct wordStruct *w2 = w;
        while (w2)
        {
            if (checkBufferBounds(&argc, &targc, &argv))
            {
                str = malloc(strlen(w2->data) + 1);
                if (str)
                {
                    strcpy(str, w2->data);
                    argv[argc++] = str;
                }
            }
            w2 = w2->next;
        }

        /* free the memory used by the expanded words */
        freeAllWords(w);

        /* check the next word */
        child = child->nextSibling;
    }

    /* even if arc == 0, we need to alloc memory for argv */
    if (checkBufferBounds(&argc, &targc, &argv))
    {
        /* NULL-terminate the array */
        argv[argc] = NULL;
    }

    int i = 0;
    for (; i < numberOfBuiltins; i++)
    {
        if (strcmp(argv[0], builtins[i].name) == 0)
        {
            builtins[i].func(argc, argv);
            freeBuffer(argc, argv);
            return 1;
        }
    }

    pid_t childPID = 0;
    if ((childPID = fork()) == 0)
    {
        doExecutionCommand(argc, argv);
        fprintf(stderr, "error: failed to execute command: %s\n", strerror(errno));
        if (errno == ENOEXEC)
        {
            exit(126);
        }
        else if (errno == ENOENT)
        {
            exit(127);
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }
    else if (childPID < 0)
    {
        fprintf(stderr, "error: failed to fork command: %s\n", strerror(errno));
        freeBuffer(argc, argv);
        return 0;
    }

    int status = 0;
    waitpid(childPID, &status, 0);
    freeBuffer(argc, argv);

    return 1;
}
