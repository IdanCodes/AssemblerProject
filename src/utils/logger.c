#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

#define LOGGER_INFO_PREFIX  "INFO"
#define LOGGER_INFO_COLOR   logColor_CYAN
#define LOGGER_WARN_PREFIX  "WARN"
#define LOGGER_WARN_COLOR   logColor_YELLOW
#define LOGGER_ERR_PREFIX   "ERR"
#define LOGGER_ERR_COLOR    logColor_RED

static void setColor(enum logColor color);
static void resetColor(void);
static void logPrintInColorWithPrefix(enum logColor color, char *prefix, char *fmt, va_list args);

/**
 * Set the current color of the logger
 * @param color The color to set to
 */
static void setColor(enum logColor color) {
    printf("\x1b[%dm", color);
}

/**
 * Reset the color of the logger
 */
static void resetColor(void) {
    setColor(logColor_RESET);
}

/**
 * Print in color with a prefix
 * @param color The color to set to
 * @param prefix The prefix to write
 * @param fmt The format
 * @param args The va_list arguments
 */
static void logPrintInColorWithPrefix(enum logColor color, char *prefix, char *fmt, va_list args) {
    setColor(color);
    printf("[%s]: ", prefix);
    vprintf(fmt, args);
    resetColor();
}

/**
 * Print a message to the console with an info prefix in the info color
 * @param fmt The format to print in
 * @param ... The arguments
 */
void logInfo(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    logPrintInColorWithPrefix(LOGGER_INFO_COLOR, LOGGER_INFO_PREFIX, fmt, args);
    
    va_end(args);
}

/**
 * Print a message to the console with an warning prefix in the warning color 
 * @param fmt The format to print in
 * @param ... The arguments
 */
void logWarn(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    logPrintInColorWithPrefix(LOGGER_WARN_COLOR, LOGGER_WARN_PREFIX, fmt, args);

    va_end(args);
}

/**
 * Print a message to the console with an error prefix in the error color
 * @param fmt The format to print in
 * @param ... The arguments
 */
void logErr(char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    logPrintInColorWithPrefix(LOGGER_ERR_COLOR, LOGGER_ERR_PREFIX, fmt, args);

    va_end(args);
}

/**
 * logErr and exit(exitCode)
 * @param exitCode The exit code to exit with
 * @param fmt The format to print in
 * @param ... The arguments
 */
void terminalError(int exitCode, char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    logPrintInColorWithPrefix(LOGGER_ERR_COLOR, LOGGER_ERR_PREFIX, fmt, args);

    va_end(args);
    /* exit will free all allocated memory in the program */
    exit(exitCode);
}

/**
 * Log an insufficient memory error with termianlError
 * @param details extra details on the insufficient memory error
 */
void logInsuffMemErr(char *details) {
    const int exitCode = 1;
    terminalError(exitCode, "Insufficient memory (%s)\n", details);
}
