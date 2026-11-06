#ifndef MONITOR_H
#define MONITOR_H

#include "service.h"

#include <stdio.h>

int monitor_run(Service *services, FILE *log_file);

#endif
