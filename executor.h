#ifndef BACKEND_H
#define BACKEND_H

#include "node.h"

char *searchPath(char *file);
int doExecutionCommand(int argc, char **argv);
int doSimpleCommand(struct nodeStruct *node);

#endif
