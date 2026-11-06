#include "dashboard.h"

#include <stdio.h>
#include <time.h>

static const char *status_name(ServiceStatus status)
{
    if (status == SERVICE_RUNNING)
        return "RUNNING";
    if (status == SERVICE_FAILED)
        return "FAILED";
    return "STOPPED";
}

void dashboard_display(Service *services, int total_crashes)
{
    int total_services = 0;
    int total_restarts = 0;
    Service *unstable_service = NULL;

    printf("\n-------------------------------------------------------------\n");
    printf("SERVICE              PID STATUS  RESTARTS HEALTH UPTIME\n");
    printf("-------------------------------------------------------------\n");

    for (Service *service = services; service != NULL; service = service->next)
    {
        int health = 100 - service->restart_count * 20;
        time_t uptime = 0;

        if (health < 0)
            health = 0;
        if (service->status == SERVICE_RUNNING)
            uptime = time(NULL) - service->start_time;
        if (unstable_service == NULL ||
            service->restart_count > unstable_service->restart_count)
            unstable_service = service;

        printf("%-20s %-4ld %-7s %-8d %3d%%   %02ld:%02ld:%02ld\n",
               service->name, (long)service->pid, status_name(service->status),
               service->restart_count, health, (long)(uptime / 3600),
               (long)((uptime / 60) % 60), (long)(uptime % 60));
        total_services++;
        total_restarts += service->restart_count;
    }

    printf("-------------------------------------------------------------\n");
    printf("Total Services : %d\n", total_services);
    printf("Total Crashes  : %d\n", total_crashes);
    printf("Total Restarts : %d\n", total_restarts);
    printf("Most Unstable  : %s\n",
           unstable_service == NULL ? "None" : unstable_service->name);
}