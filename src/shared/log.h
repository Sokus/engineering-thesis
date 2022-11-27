#ifndef PI_LOG_H
#define PI_LOG_H

#define MAX_LOG_MESSAGE_LENGTH 128

enum LogLevel
{
    LOG_ALL = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_NONE,
};

void SetLogLevel(int log_level);
void SetLogPrefix(const char *prefix);
void Log(int log_level, const char *text, ...);

#endif // PI_LOG_H
