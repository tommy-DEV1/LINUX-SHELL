#include <unistd.h>
#include "shell.h"
#include "parser.h"
#include "scanner.h"
#include "node.h"
#include "source.h"

struct nodeStruct *parseSimpleCommand(struct tokenStruct *tok)
{
    if (!tok)
    {
        return NULL;
    }

    struct nodeStruct *cmd = newNode(NODE_COMMAND);
    if (!cmd)
    {
        freeToken(tok);
        return NULL;
    }

    struct sourceStruct *src = tok->src;

    do
    {
        if (tok->text[0] == '\n')
        {
            freeToken(tok);
            break;
        }

        struct nodeStruct *word = newNode(NODE_VAR);
        if (!word)
        {
            freeNodeTree(cmd);
            freeToken(tok);
            return NULL;
        }
        setNodeValueString(word, tok->text);
        addChildNode(cmd, word);

        freeToken(tok);

    } while ((tok = tokenize(src)) != &EOFToken);

    return cmd;
}
