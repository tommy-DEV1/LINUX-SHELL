#include "../shell.h"
#include <stdio.h>

struct builtinStruct builtins[] = {
    {"dump", byteShellDump},
    {"cd", byteShellCd},
    {"help", byteShellHelp},
};

int numberOfBuiltins = sizeof(builtins) / sizeof(struct builtinStruct);

int byteShellHelp(int argc, char **args)
{
    int i;
    printf("This is Shrey Gupta's ByteShell\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following (apart from exit) are built in :\n");

    for (i = 0; i < numberOfBuiltins; i++)
    {
        printf("  %s\n", builtins[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}
