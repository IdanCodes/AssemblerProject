#ifndef PRE_ASM
#define PRE_ASM

#include <stdio.h>

#define DEC_MCR_INSTRUCTION "mcr"
#define END_MCR_INSTRUCTION "endmcr"

typedef struct MACRO {
    char *name;
    unsigned int startLine;
    unsigned int endLine;
    struct MACRO *next;
} MACRO;

enum preAssembleErr {
    preAssembleErr_no_err,
    preAssembleErr_mcr_expected,
    preAssembleErr_unexpected_chars_dec,
    preAssembleErr_unexpected_chars_end,
    preAssembleErr_unexpected_chars_ref,
    preAssembleErr_macro_exists
};

/* TODO: remove this, only return 0 and 1 (if the enum is binary) */
enum preAssembleStatus {
    preAssemble_OK,
    preAssemble_ERROR
};

enum preAssembleStatus preAssemble(char fileName[]);
char *preAsmErrMessage(enum preAssembleErr err);
MACRO *allocMcr(char *name);
void freeMcr(MACRO *mcr);
MACRO *getMacroWithName(char *name, MACRO *head);
void expandMacro(MACRO *mcr, FILE *destf, char *sourcefileName); /* use fprintf */

#endif /* PRE_ASM */
