#ifndef LOGGER
#define LOGGER

/*
 * #define logColor_BLACK "30"
#define logColor_RED "31"
#define logColor_GREEN "32"
#define logColor_YELLOW "33"
#define logColor_BLUE "34"
#define logColor_MAGENTA "35"
#define logColor_CYAN "36"
#define logColor_WHITE "37"
#define logColor_RESET "38"
 */

/* TODO: add other colors */

enum logColor {
    logColor_BLACK =    30,
    logColor_RED =      31,
    logColor_GREEN =    32,
    logColor_YELLOW =   33,
    logColor_BLUE =     34,
    logColor_MAGENTA =  35,
    logColor_CYAN =     36,
    logColor_RESET =    0
};

void logPrint(char *fmt, ...);
void logInfo(char *fmt, ...);
void logWarn(char *fmt, ...);
void logError(char *fmt, ...);
void terminalError(int exitCode, char *fmt, ...);

#endif /* LOGGER */
