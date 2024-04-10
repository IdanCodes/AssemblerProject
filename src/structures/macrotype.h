#ifndef MACROTYPE
#define MACROTYPE

#include <stdio.h>

typedef struct Macro {
    char *name;
    unsigned int startLine;
    unsigned int endLine;
    struct Macro *next;
} Macro;

Macro *allocMcr(char *name);
void freeMcr(Macro *mcr);
Macro *getMacroWithName(char *name, Macro *head);
void expandMacro(Macro *mcr, FILE *destf, char *sourcefileName); /* use fprintf */

#endif /* MACROTYPE */
