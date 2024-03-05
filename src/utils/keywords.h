#ifndef KEYWORDS
#define KEYWORDS

#define KEYWORD_MCR_DEC "mcr"
#define KEYWORD_MCR_END "endmcr"

#define KEYWORD_CONST_DEC   ".define"
#define KEYWORD_DATA_DEC    ".data"
#define KEYWORD_STRING_DEC  ".string"

#define KEYWORD_REGISTER_PREFIX "r"
#define REGISTER_FIRST  1
#define REGISTER_COUNT  8

#define KEYWORD_OPERATION_MOV   "mov"
#define KEYWORD_OPERATION_CMP   "cmp"
#define KEYWORD_OPERATION_ADD   "add"
#define KEYWORD_OPERATION_SUB   "sub"
#define KEYWORD_OPERATION_NOT   "not"
#define KEYWORD_OPERATION_CLR   "clr"
#define KEYWORD_OPERATION_LEA   "lea"
#define KEYWORD_OPERATION_INC   "inc"
#define KEYWORD_OPERATION_DEC   "dec"
#define KEYWORD_OPERATION_JMP   "jmp"
#define KEYWORD_OPERATION_BNE   "bne"
#define KEYWORD_OPERATION_RED   "red"
#define KEYWORD_OPERATION_PRN   "prn"
#define KEYWORD_OPERATION_JSR   "jsr"
#define KEYWORD_OPERATION_RTS   "rts"
#define KEYWORD_OPERATION_HLT   "hlt"

int isRegisterName(char *keyword, int *regIndex);
int isSavedKeyword(char *keyword);

#endif /* KEYWORDS */
