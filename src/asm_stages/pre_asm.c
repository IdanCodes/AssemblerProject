#include "pre_asm.h"
#include "../utils/inpututils.h"
#include "../utils/logger.h"
#include "../utils/strutils.h"

enum preAssembleStatus preAssemble(FILE *fp) {
    unsigned int lineNumber;
    char line[MAXLINE];
    int err, len;
    
    lineNumber = 0;
    while ((err = getLine(fp, line, MAXLINE, &len)) != getLine_FILE_END) {
        lineNumber++;
        
        if (err == getLine_TOO_LONG) {
            logError("Line %u is too long (allowed length is %d characters).\n", lineNumber, MAXLINE);
            return preAssemble_ERROR;
        }
        
        len = trim(line);
        if (err == getLine_COMMENT || len == 0)
            continue;
        
        logPrint("%u. %s\n", lineNumber, line);
    }
    
    return preAssemble_OK;
}
