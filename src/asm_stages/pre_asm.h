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
    preAssembleErr_mcr_expected,    /* macro name expected */
    preAssembleErr_unexpected_chars_dec,    /* unexpected characters after macro name in macro declaration */
    preAssembleErr_unexpected_chars_end,    /* unexpected characters after "endmcr" */
    preAssembleErr_unexpected_chars_call,   /* unexpected characters after macro call */ 
    preAssembleErr_macro_exists,    /* a macro with this name already exists */
    preAssembleErr_macro_saved_name,    /* the name of the macro is a saved name */
    preAssembleErr_unexpected_end   /* unexpected endmcr outside of a macro */
};

enum preAssembleErr preAssemble(char fileName[]);
char *preAsmErrMessage(enum preAssembleErr err);
Macro *allocMcr(char *name);
void freeMcr(Macro *mcr);
Macro *getMacroWithName(char *name, Macro *head);
void expandMacro(Macro *mcr, FILE *destf, char *sourcefileName); /* use fprintf */

#endif /* PRE_ASM */
