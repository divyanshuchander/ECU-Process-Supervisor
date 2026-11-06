#ifndef CONFIG_H
#define CONFIG_H

#include "service.h"

int config_load(const char *path, Service **services);
void config_free(Service *services);

#endif
