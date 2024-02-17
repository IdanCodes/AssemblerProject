#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

/* TODO: Document these methods */

void logPrint(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);    /* initialize args */
    fprintf(stderr, "");
    vfprintf(stdin, fmt, args);    /* use 'vfprintf' to handle formatting */
    va_end(args);           /* close the args */
}

void logInfo(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);    /* initialize args */
    fprintf(stderr, "[INFO]: ");
    vfprintf(stderr, fmt, args);    /* use 'vfprintf' to handle formatting */
    va_end(args);           /* close the args */
}

void logWarn(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);    /* initialize args */
    fprintf(stderr, "[WARN]: ");
    vfprintf(stderr, fmt, args);    /* use 'vfprintf' to handle formatting */
    va_end(args);           /* close the args */
}

void logError(char *fmt, ...) {
    va_list args;

    va_start(args, fmt);    /* initialize args */
    fprintf(stderr, "[ERR]: ");
    vfprintf(stderr, fmt, args);    /* use 'vfprintf' to handle formatting */
    va_end(args);           /* close the args */
}

/* print & exit */
void terminalError(int exitCode, char *fmt, ...) {
    va_list args;

    va_start(args, fmt);    /* initialize args */
    fprintf(stderr, "[ERR]: ");
    vfprintf(stderr, fmt, args);    /* use 'vfprintf' to handle formatting */
    va_end(args);           /* close the args */
    
    exit(exitCode);
}
