#include "pre_asm.h"
#include "../utils/inpututils.h"
#include "../utils/logger.h"
#include "../utils/strutils.h"
#include "../utils/fileutils.h"
#include "../utils/charutils.h"

/* DOCUMENT preAssemble */
/* fileName is the name of the file without the added extension */
enum preAssembleStatus preAssemble(char fileName[]) {
    unsigned int sourceLine;
    char line[MAXLINE+1];   /* account for '\0' */
    int getLineErr, len;
    char sourceFileName[FILENAME_MAX], outFilePath[FILENAME_MAX];
    FILE *sourcef, *outf;

    
    /* -- open source and output files -- */
    sprintf(sourceFileName, "%s.%s", fileName, SOURCE_FILE_EXTENSION);
    sprintf(outFilePath, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);

    openFile(sourceFileName, "r", &sourcef);    /* open requested file for reading */
    openFile(outFilePath, "w", &outf);      /* open pre-assembled file for writing */
    
    sourceLine = 0;
    while ((getLineErr = getLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine++;
        
        if (getLineErr == getLine_TOO_LONG) {
            logWarn("Line %u in file '%s' is too long! Ignoring line (maximum size is %d characters).\n", sourceLine, sourceFileName, MAXLINE);
            continue;
        }
        else if ((len = trim(line) == 0) || line[0] == COMMENT_CHAR)
            continue;   /* empty or comment - skip */
        
        fprintf(outf, "%s\n", line);
    }
    
    /* close opened files */
    fclose(sourcef);
    fclose(outf);
    
    return preAssemble_OK;
}
