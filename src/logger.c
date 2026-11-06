#include "logger.h"

#include <stdarg.h>
#include <string.h>
#include <time.h>

int logger_open(FILE **log_file, const char *path)
{
    if (log_file == NULL || path == NULL)
    {
        return -1;
    }

    *log_file = fopen(path, "a");
    if (*log_file == NULL)
    {
        return -1;
    }

    return 0;
}

void logger_close(FILE *log_file)
{
    if (log_file != NULL)
    {
        fclose(log_file);
    }
}

int logger_log(FILE *log_file, const char *format, ...)
{
    if (log_file == NULL || format == NULL)
    {
        return -1;
    }

    time_t current_time = time(NULL);
    struct tm *timestamp = localtime(&current_time);
    if (timestamp == NULL)
    {
        return -1;
    }

    char timestamp_text[32];
    if (strftime(timestamp_text, sizeof(timestamp_text),
                 "%Y-%m-%d %H:%M:%S", timestamp) == 0)
    {
        return -1;
    }

    if (fprintf(log_file, "[%s] ", timestamp_text) < 0)
    {
        return -1;
    }

    va_list arguments;
    va_start(arguments, format);
    int result = vfprintf(log_file, format, arguments);
    va_end(arguments);

    if (result < 0 || fputc('\n', log_file) == EOF ||
        fflush(log_file) == EOF)
    {
        return -1;
    }

    return 0;
}