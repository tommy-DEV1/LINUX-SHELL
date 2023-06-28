#ifndef SHELL_H
#define SHELL_H

#include <stddef.h> /* size_t */
#include <glob.h>
#include "source.h"

void printPrompt1(void);
void printPrompt2(void);
char *readCommand(void);
int parseAndExecute(struct sourceStruct *src);

void initsh(void);

/* shell builtin utilities */
int byteShellDump(int argc, char **argv);
int byteShellCd(int argc, char **args);
int byteShellHelp(int argc, char **args);

/* struct for builtin utilities */
struct builtinStruct
{
    char *name;                         /* utility name */
    int (*func)(int argc, char **argv); /* function to call to execute the utility */
};

/* the list of builtin utilities */
extern struct builtinStruct builtins[];

/* and their count */
extern int numberOfBuiltins;

/* struct to represent the words resulting from word expansion */
struct wordStruct
{
    char *data;
    int len;
    struct wordStruct *next;
};

/* word expansion functions */
struct wordStruct *makeWord(char *word);
void freeAllWords(struct wordStruct *first);

size_t findClosingQuote(char *data);
size_t findClosingBrace(char *data);
void deleteCharacterAt(char *str, size_t index);
char *substituteString(char *s1, char *s2, size_t start, size_t end);
char *wordlistToString(struct wordStruct *word);

struct wordStruct *wordExpand(char *originalWord);
char *wordExpandToString(char *word);
char *tildeExpand(char *s);
char *commandSubstitute(char *command);
char *variableExpand(char *variableName);
struct wordStruct *pathNamesExpand(struct wordStruct *words);
struct wordStruct *fieldSplit(char *str);
void removeQuotes(struct wordStruct *wordlist);

char *arithmaticExpand(char *expression);

/* some string manipulation functions */
char *stringCharacterAny(char *string, char *chars);
char *quoteValue(char *val, int addQuotes);
int checkBufferBounds(int *count, int *len, char ***buf);
void freeBuffer(int len, char **buf);

/* pattern matching functions */
int hasGlobarCharacters(char *p, size_t len);
int matchPrefix(char *pattern, char *str, int longest);
int matchSuffix(char *pattern, char *str, int longest);
char **getFileNameMatches(char *pattern, glob_t *matches);

#endif
