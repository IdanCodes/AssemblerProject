#ifndef LOGGER
#define LOGGER

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
void logErr(char *fmt, ...);
void terminalError(int exitCode, char *fmt, ...);

#endif /* LOGGER */
