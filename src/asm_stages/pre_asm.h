#ifndef PRE_ASM
#define PRE_ASM

#include <stdio.h>
#include "../structures/macrotype.h"

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

int preAssemble(char fileName[FILENAME_MAX], Macro **macros);

#endif /* PRE_ASM */
