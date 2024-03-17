#ifndef PRE_ASM
#define PRE_ASM

#include <stdio.h>

typedef struct Macro {
    char *name;
    unsigned int startLine;
    unsigned int endLine;
    struct Macro *next;
} Macro;

enum preAssembleErr {
    preAssembleErr_no_err,
    preAssembleErr_mcr_expected,
    preAssembleErr_unexpected_chars_dec,
    preAssembleErr_unexpected_chars_end,
    preAssembleErr_unexpected_chars_ref,
    preAssembleErr_macro_exists,
    preAssembleErr_macro_saved_name
};

enum preAssembleErr preAssemble(char fileName[]);
char *preAsmErrMessage(enum preAssembleErr err);
Macro *allocMcr(char *name);
void freeMcr(Macro *mcr);
Macro *getMacroWithName(char *name, Macro *head);
void expandMacro(Macro *mcr, FILE *destf, char *sourcefileName); /* use fprintf */

#endif /* PRE_ASM */
