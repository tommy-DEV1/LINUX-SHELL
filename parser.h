#ifndef PARSER_H
#define PARSER_H

#include "scanner.h" /* struct tokenStruct */
#include "source.h"  /* struct sourceStruct */

struct nodeStruct *parseSimpleCommand(struct tokenStruct *tok);

#endif
