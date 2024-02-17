#ifndef LOGGER
#define LOGGER

void logPrint(char *fmt, ...);
void logInfo(char *fmt, ...);
void logWarn(char *fmt, ...);
void logError(char *fmt, ...);
void terminalError(int exitCode, char *fmt, ...);

#endif /* LOGGER */
