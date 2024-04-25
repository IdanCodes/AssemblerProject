#include "keywords.h"
#include "operations.h"
#include "strutils.h"

/**
 * Checks if a keyword is a register name
 * NOTE: a register like r10 will NOT be considered a register with an invalid register "index"
 * @param keyword The keyword to check
 * @param regIndex A pointer to the register index
 * @return Whether the keyword is a register name
 */
int isRegisterName(char *keyword, int *regIndex) {
    char *tmpPrefix = KEYWORD_REGISTER_PREFIX;
    
    for (; *tmpPrefix != '\0' && *keyword != '\0' && *tmpPrefix == *keyword; tmpPrefix++, keyword++)
        ;   /* keep reading */
    
    return *tmpPrefix == '\0' &&    /* match prefix */
            tryParseToken(keyword, regIndex) && /* is a valid number */
            *regIndex >= REGISTER_FIRST && *regIndex < REGISTER_FIRST + REGISTER_COUNT; /* register index in range */
}

/**
 * Checks if a keyword is a saved keyword
 * Note: compares the first token of the given string
 * @param keyword The keyword to check
 * @return Whether the keyword is a saved keyword
 */
int isSavedKeyword(char *keyword) {
    int tempInt;
    Operation tempOp;
    
    return isRegisterName(keyword, &tempInt) ||    /* register */
           getOperationByName(keyword, &tempOp) || /* operation */
           tokcmp(keyword, KEYWORD_MCR_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_MCR_END) == 0 ||
           tokcmp(keyword, KEYWORD_CONST_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_DATA_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_STRING_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_ENTRY_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_EXTERN_DEC) == 0;
}
