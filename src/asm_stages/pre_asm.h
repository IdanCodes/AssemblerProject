#ifndef PRE_ASM
#define PRE_ASM

#include <stdio.h>
#include "../structures/macrotype.h"

enum preAssembleErr {
    preAssembleErr_no_err,
    preAssembleErr_mcr_expected,    /* macro name expected */   /* DONE */
    preAssembleErr_macro_invalid_name,  /* the macro's name is invalid */   /* DONE */
    preAssembleErr_unexpected_chars_dec,    /* unexpected characters after macro name in macro declaration */   /* DONE */
    preAssembleErr_unexpected_chars_end,    /* unexpected characters after "endmcr" */  /* DONE */
    preAssembleErr_unexpected_chars_call,   /* unexpected characters after macro call */    /* DONE */
    preAssembleErr_macro_exists,    /* a macro with this name already exists */ /* DONE */
    preAssembleErr_macro_saved_keyword, /* the name of the macro is a saved keyword */ /* DONE */
    preAssembleErr_unexpected_end   /* unexpected endmcr outside of a macro */  /* DONE */
};

int preAssemble(char fileName[FILENAME_MAX], Macro **macros);

#endif /* PRE_ASM */
