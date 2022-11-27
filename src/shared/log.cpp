#include "log.h"

#include <stdlib.h> // exit()
#include <stdio.h> // fprintf(), vprintf()
#include <stdarg.h> // va_list, va_start(), va_end()
#include <string.h> // strcpy(), strcat()

static int g_log_level = LOG_INFO;
static const char *g_prefix = 0;
static size_t g_prefix_length = 0;

void SetLogLevel(int log_level)
{
    g_log_level = log_level;
}

void SetLogPrefix(const char *prefix)
{
    g_prefix = prefix;
    g_prefix_length = strlen(prefix);
}

void Log(int log_level, const char *text, ...)
{
    if (log_level < g_log_level) return;

    va_list args;
    va_start(args, text);

    char buffer[MAX_LOG_MESSAGE_LENGTH] = {0};
    if(g_prefix_length > 0)
        strcat(buffer, g_prefix);

    switch (log_level)
    {
        case LOG_DEBUG:   strcat(buffer, "DEBUG: "); break;
        case LOG_INFO:    strcat(buffer, "INFO: "); break;
        case LOG_WARNING: strcat(buffer, "WARNING: "); break;
        case LOG_ERROR:   strcat(buffer, "ERROR: "); break;
        default: break;
    }

    strcat(buffer, text);
    strcat(buffer, "\n");
    vprintf(buffer, args);
    fflush(stdout);

    va_end(args);
}