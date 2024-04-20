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
Macro *getMacroWithName(char *name, Macro *head);
void expandMacro(Macro *mcr, FILE *destf, char *sourcefileName); /* use fprintf */
void freeMcr(Macro *mcr);
void freeMcrList(Macro *head);

#endif /* MACROTYPE */
