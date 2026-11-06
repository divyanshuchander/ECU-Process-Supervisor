#include "monitor.h"

#include "launcher.h"
#include "logger.h"
#include "dashboard.h"

#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t child_finished;
static volatile sig_atomic_t shutdown_requested;

static void handle_child_signal(int signal_number)
{
    (void)signal_number;
    child_finished = 1;
}

static void handle_shutdown_signal(int signal_number)
{
    (void)signal_number;
    shutdown_requested = 1;
}

static int install_signal_handlers(void)
{
    if (signal(SIGCHLD, handle_child_signal) == SIG_ERR ||
        signal(SIGINT, handle_shutdown_signal) == SIG_ERR ||
        signal(SIGTERM, handle_shutdown_signal) == SIG_ERR)
    {
        fprintf(stderr, "Could not install signal handlers\n");
        return -1;
    }

    return 0;
}

static void check_children(Service *services, FILE *log_file, int *total_crashes)
{
    for (Service *service = services; service != NULL; service = service->next)
    {
        if (service->pid <= 0)
        {
            continue;
        }

        int status;
        pid_t result = waitpid(service->pid, &status, WNOHANG);
        if (result == 0)
        {
            continue;
        }
        if (result < 0)
        {
            continue;
        }

        pid_t old_pid = service->pid;
        if (WIFSIGNALED(status))
        {
            logger_log(log_file, "%s Crashed PID=%ld",
                       service->name, (long)old_pid);
            (*total_crashes)++;
        }
        else
            logger_log(log_file, "%s Stopped PID=%ld",
                       service->name, (long)old_pid);

        service->pid = -1;
        if (service->restart_count < service->max_restart)
        {
            service->restart_count++;
            if (launcher_start(service, log_file) == 0)
                logger_log(log_file, "%s Restarted PID=%ld",
                           service->name, (long)service->pid);
            else
                service->status = SERVICE_FAILED;
        }
        else
        {
            service->status = SERVICE_FAILED;
            logger_log(log_file, "%s Failed", service->name);
        }
    }
}

int monitor_run(Service *services, FILE *log_file)
{
    int total_crashes = 0;

    child_finished = 0;
    shutdown_requested = 0;

    if (install_signal_handlers() != 0)
    {
        return -1;
    }

    for (Service *service = services; service != NULL; service = service->next)
    {
        if (launcher_start(service, log_file) != 0)
        {
            service->status = SERVICE_FAILED;
        }
    }

    while (!shutdown_requested)
    {
        if (child_finished)
        {
            child_finished = 0;
            check_children(services, log_file, &total_crashes);
        }
        dashboard_display(services, total_crashes);
        sleep(1);
    }

    launcher_stop_all(services, log_file);
    return 0;
}