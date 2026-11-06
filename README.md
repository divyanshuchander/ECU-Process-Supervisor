# ECU Process Supervisor

This repository contains a small Linux process-supervision proof of concept
written in C. The supervisor will read service definitions, launch and monitor
child processes, restart failed services, maintain logs, and display a terminal
dashboard.

The project intentionally avoids threads, networking, databases, external
configuration parsers, and systemd APIs. See [docs/Context.md](docs/Context.md)
for the complete requirements.

## Project layout

```text
include/   Public module headers
src/       Supervisor implementation
services/  Dummy loop services
config/    Service configuration
logs/      Runtime logs
```

## Build

The project uses GCC and GNU Make:

```sh
make
make clean
```

All build artifacts are placed under `build/`. The supervisor is built as
`build/supervisor`, and the dummy services are built under `build/services/`.

## Service configuration

Each non-empty, non-comment line in
[config/services.conf](config/services.conf) contains:

```text
ServiceName ExecutablePath MaxRestartCount
```

The loader rejects malformed lines, duplicate service names, and negative
restart limits. Configuration errors stop startup rather than allowing a
partially loaded service list to run.

The dummy services run silent infinite `while` loops with `sleep()`. They do
not perform any ADAS processing and are intended only as child processes for
the supervisor, so their output does not interfere with the dashboard.

The current implementation also monitors child exits, restarts services up to
their configured limit, and handles `SIGINT` and `SIGTERM` for graceful
shutdown. The terminal dashboard displays service status, uptime, health,
restart counts, and crash statistics.

## Demonstration

Build and start the supervisor from the project root:

```sh
make
./build/supervisor
```

Find a service PID in the dashboard or with:

```sh
ps --ppid <supervisor-pid> -o pid,comm
```

Simulate a crash:

```sh
kill -9 <service-pid>
```

The supervisor logs the crash, restarts the service, changes its PID, increases
its restart count, and reduces its health score. Press `Ctrl+C` to stop the
supervisor and all managed services.
