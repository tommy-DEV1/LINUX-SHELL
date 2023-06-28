#include "../shell.h"
#include <stdio.h>

int byteShellCd(int argc, char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "ByteShell: expected argument to \"cd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("ByteShell");
        }
    }
    return 1;
}