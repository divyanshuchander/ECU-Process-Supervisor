#ifndef SERVICE_H
#define SERVICE_H

#include <sys/types.h>
#include <time.h>

#define SERVICE_NAME_MAX 64
#define SERVICE_PATH_MAX 256

typedef enum
{
    SERVICE_RUNNING,
    SERVICE_FAILED,
    SERVICE_STOPPED
} ServiceStatus;

typedef struct Service
{
    char name[SERVICE_NAME_MAX];
    char path[SERVICE_PATH_MAX];
    pid_t pid;
    ServiceStatus status;
    int restart_count;
    int max_restart;
    time_t start_time;
    struct Service *next;
} Service;

#endif
