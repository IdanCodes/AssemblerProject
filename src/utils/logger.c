#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

/* DOCUMENT logger */

#define LOGGER_INFO_PREFIX  "INFO"
#define LOGGER_INFO_COLOR   logColor_CYAN
#define LOGGER_WARN_PREFIX  "WARN"
#define LOGGER_WARN_COLOR   logColor_YELLOW
#define LOGGER_ERR_PREFIX   "ERR"
#define LOGGER_ERR_COLOR    logColor_RED

static void setColor(enum logColor color);
static void resetColor(void);
static void logPrintInColorWithPrefix(enum logColor color, char *prefix, char *fmt, va_list args);

static void setColor(enum logColor color) {
    printf("\x1b[%dm", color);
}

static void resetColor(void) {
    setColor(logColor_RESET);
}
static void logPrintInColorWithPrefix(enum logColor color, char *prefix, char *fmt, va_list args) {
    setColor(color);
    printf("[%s]: ", prefix);
    vprintf(fmt, args);
    resetColor();
}

void logPrint(char *fmt, ...) {
    va_list args;
    
    va_start(args, fmt);
    resetColor();
    vprintf(fmt, args);
    va_end(args);
}

void logInfo(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    logPrintInColorWithPrefix(LOGGER_INFO_COLOR, LOGGER_INFO_PREFIX, fmt, args);
    
    va_end(args);
}

void logWarn(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    logPrintInColorWithPrefix(LOGGER_WARN_COLOR, LOGGER_WARN_PREFIX, fmt, args);

    va_end(args);
}

void logErr(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    logPrintInColorWithPrefix(LOGGER_ERR_COLOR, LOGGER_ERR_PREFIX, fmt, args);

    va_end(args);
}

void terminalError(int exitCode, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    logPrintInColorWithPrefix(LOGGER_ERR_COLOR, LOGGER_ERR_PREFIX, fmt, args);

    va_end(args);
    exit(exitCode);
}

void logInsuffMemErr(char *details) {
    const int exitCode = 1;
    terminalError(exitCode, "Insufficient memory (%s)\n", details);
}
