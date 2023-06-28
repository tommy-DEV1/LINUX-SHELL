#include "../shell.h"
#include "../symtab/symtab.h"

int byteShellDump(int argc, char **argv)
{
    dumpLocalSymtab();
    return 0;
}
