#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int config_load(const char *path, Service **services)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        perror("Could not open configuration");
        return -1;
    }

    Service *last = *services;
    while (last != NULL && last->next != NULL)
    {
        last = last->next;
    }
    char line[512];
    unsigned int line_number = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char name[SERVICE_NAME_MAX];
        char executable_path[SERVICE_PATH_MAX];
        int max_restart;
        line_number++;

        if (line[0] == '\n' || line[0] == '#')
        {
            continue;
        }

        if (sscanf(line, "%63s %255s %d",
                   name, executable_path, &max_restart) != 3 ||
            max_restart < 0)
        {
            fprintf(stderr, "Invalid configuration at line %u\n", line_number);
            config_free(*services);
            fclose(file);
            *services = NULL;
            return -1;
        }

        for (Service *service = *services; service != NULL;
             service = service->next)
        {
            if (strcmp(service->name, name) == 0)
            {
                fprintf(stderr, "Duplicate service at line %u\n", line_number);
                config_free(*services);
                fclose(file);
                *services = NULL;
                return -1;
            }
        }

        Service *service = calloc(1, sizeof(*service));
        if (service == NULL)
        {
            fprintf(stderr, "Could not allocate service\n");
            config_free(*services);
            fclose(file);
            *services = NULL;
            return -1;
        }

        strcpy(service->name, name);
        strcpy(service->path, executable_path);
        service->pid = -1;
        service->status = SERVICE_STOPPED;
        service->max_restart = max_restart;

        if (*services == NULL)
        {
            *services = service;
        }
        else
        {
            last->next = service;
        }
        last = service;
    }

    fclose(file);
    return 0;
}

void config_free(Service *services)
{
    while (services != NULL)
    {
        Service *next = services->next;
        free(services);
        services = next;
    }
}
