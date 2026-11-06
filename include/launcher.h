#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "service.h"

#include <stdio.h>

int launcher_start(Service *service, FILE *log_file);
void launcher_stop(Service *service, FILE *log_file);
void launcher_stop_all(Service *services, FILE *log_file);

#endif
