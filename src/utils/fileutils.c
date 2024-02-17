#include <stdio.h>
#include "fileutils.h"
#include "logger.h"

#define EXIT_CODE_INVALID_FILE (-1)

/**
 * Open a file with error handling - exits if an error has occurres
 * @param fileName the full name of the file to open (including file extension)
 * @param mode file open mode
 * @param pfile a FILE* address to place the requested file's FILE* into
 * @return a FILE* to the requested file
 */
void openFile(char fileName[], char mode[], FILE **pfile) {
    if (!tryOpenFile(fileName, mode, pfile))
        terminalError(EXIT_CODE_INVALID_FILE, "Insufficient permissions to open '%s', or it doesn't exist.\n", fileName);
}

/**
 * Safely open a file
 * @param fileName the full name of the file to open (including file extension)
 * @param mode file open mode
 * @param pfile a FILE* address to place the requested file's FILE* into
 * @return if there was a problem opening the file (insuff perms or file doesn't exist), returns 0.
 * otherwise, returns 1 and sets 'pfile' to point at the requested FILE*
 */
int tryOpenFile(char fileName[], char mode[], FILE **pfile) {
    return (*pfile = fopen(fileName, mode)) != NULL;
}
