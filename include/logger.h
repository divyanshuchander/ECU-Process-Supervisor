#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

int logger_open(FILE **log_file, const char *path);
void logger_close(FILE *log_file);
int logger_log(FILE *log_file, const char *format, ...);

#endif
