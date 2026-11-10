#include <gtest/gtest.h>

#include <csignal>
#include <cstdio>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

extern "C" {
#include "monitor.h"
}

namespace
{

int RunMonitorUntilShutdown(const char *path, int max_restart)
{
    pid_t signal_sender = fork();
    if (signal_sender == 0)
    {
        sleep(1);
        kill(getppid(), SIGTERM);
        _exit(0);
    }

    if (signal_sender < 0)
        return -1;

    Service service{};
    std::snprintf(service.name, SERVICE_NAME_MAX, "%s", "TestService");
    std::snprintf(service.path, SERVICE_PATH_MAX, "%s", path);
    service.pid = -1;
    service.max_restart = max_restart;

    FILE *log_file = std::tmpfile();
    if (log_file == nullptr)
        return -1;

    const int result = monitor_run(&service, log_file);
    std::fclose(log_file);

    int status = 0;
    if (waitpid(signal_sender, &status, 0) != signal_sender)
        return -1;
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        return -1;

    return result;
}

TEST(MonitorTest, MarksNormallyExitedServiceAsFailedAtRestartLimit)
{
    EXPECT_EQ(RunMonitorUntilShutdown("/bin/false", 0), 0);
}

TEST(MonitorTest, HandlesFailedServiceStart)
{
    EXPECT_EQ(RunMonitorUntilShutdown("/does/not/exist", 0), 0);
}

TEST(MonitorTest, RestartsSignaledServiceUntilLimit)
{
    const std::string script_path = "gtest/crash-service.sh";
    {
        std::ofstream script(script_path);
        script << "#!/bin/sh\nkill -SEGV $$\n";
    }
    ASSERT_EQ(chmod(script_path.c_str(), 0755), 0);

    EXPECT_EQ(RunMonitorUntilShutdown(script_path.c_str(), 1), 0);

    std::remove(script_path.c_str());
}

}  // namespace
