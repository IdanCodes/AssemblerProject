#include "keywords.h"
#include "operations.h"
#include "strutils.h"

/* DOCUMENT */
int isRegisterName(char *keyword, int *regIndex) {
    char *tmpPrefix = KEYWORD_REGISTER_PREFIX;
    
    for (; *tmpPrefix != '\0' && *keyword != '\0' && *tmpPrefix == *keyword; tmpPrefix++, keyword++)
        ;   /* keep reading */
    
    return *tmpPrefix == '\0' &&    /* match prefix */
            tryParseToken(keyword, regIndex) && /* is a valid number */
            *regIndex >= REGISTER_FIRST && *regIndex < REGISTER_FIRST + REGISTER_COUNT; /* register index in range */
}

int isSavedKeyword(char *keyword) {
    int tempInt;
    Operation tempOp;
    
    return isRegisterName(keyword, &tempInt) ||    /* register */
           getOperationByName(keyword, &tempOp) || /* operation */
           tokcmp(keyword, KEYWORD_MCR_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_MCR_END) == 0 ||
           tokcmp(keyword, KEYWORD_CONST_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_DATA_DEC) == 0 ||
           tokcmp(keyword, KEYWORD_STRING_DEC) == 0;
}
