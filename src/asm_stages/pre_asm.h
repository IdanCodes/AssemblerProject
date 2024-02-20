#ifndef PRE_ASM
#define PRE_ASM

#include <stdio.h>

/* TODO: remove this, only return 0 and 1 (if the enum is binary) */
enum preAssembleStatus {
    preAssemble_OK,
    preAssemble_ERROR
};

enum preAssembleStatus preAssemble(char fileName[]);

#endif /* PRE_ASM */
