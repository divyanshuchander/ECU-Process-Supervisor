#include "launcher.h"

#include "logger.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int launcher_start(Service *service, FILE *log_file)
{
    if (service == NULL)
    {
        return -1;
    }

    char *const arguments[] = {service->path, NULL};
    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        fprintf(stderr, "Failed to start %s: %s\n",
                service->name, strerror(errno));
        (void)logger_log(log_file, "%s Start Failed: %s",
                         service->name, strerror(errno));
        return -1;
    }

    if (child_pid == 0)
    {
        execvp(service->path, arguments);
        fprintf(stderr, "Failed to execute %s: %s\n",
                service->path, strerror(errno));
        _exit(127);
    }

    service->pid = child_pid;
    service->status = SERVICE_RUNNING;
    service->start_time = time(NULL);
    if (logger_log(log_file, "%s Started PID=%ld",
                   service->name, (long)service->pid) != 0)
    {
        fprintf(stderr, "Failed to log start of %s\n", service->name);
    }

    return 0;
}

void launcher_stop(Service *service, FILE *log_file)
{
    if (service == NULL || service->pid <= 0)
    {
        return;
    }

    if (kill(service->pid, SIGTERM) != 0 && errno != ESRCH)
    {
        fprintf(stderr, "Failed to stop %s: %s\n",
                service->name, strerror(errno));
    }
    else
    {
        while (waitpid(service->pid, NULL, 0) < 0)
        {
            if (errno != EINTR)
            {
                fprintf(stderr, "Failed to reap %s: %s\n",
                        service->name, strerror(errno));
                break;
            }
        }
    }

    (void)logger_log(log_file, "%s Stopped PID=%ld",
                     service->name, (long)service->pid);
    service->pid = -1;
    service->status = SERVICE_STOPPED;
}

void launcher_stop_all(Service *services, FILE *log_file)
{
    for (Service *service = services; service != NULL; service = service->next)
    {
        launcher_stop(service, log_file);
    }
}