#ifndef BASE_LOG_H
#define BASE_LOG_H

#define MAX_LOG_MESSAGE_LENGTH 128

enum LogLevel
{
    LOG_ALL = 0,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NONE,
};

void SetLogLevel(int log_level);
void Log(int log_level, const char *text, ...);

#endif // BASE_LOG_H