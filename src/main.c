#include "config.h"
#include "logger.h"
#include "monitor.h"

#include <stdio.h>

int main(void)
{
    Service *services = NULL;
    FILE *log_file = NULL;

    if (config_load("config/services.conf", &services) != 0)
    {
        return 1;
    }

    if (logger_open(&log_file, "logs/supervisor.log") != 0)
    {
        fprintf(stderr, "Could not open supervisor log\n");
        config_free(services);
        return 1;
    }

    logger_log(log_file, "Supervisor Started");
    monitor_run(services, log_file);
    logger_log(log_file, "Supervisor Shutdown");

    logger_close(log_file);
    config_free(services);
    return 0;
}
