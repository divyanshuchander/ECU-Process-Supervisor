#include <gtest/gtest.h>

#include <cstdio>
#include <sys/wait.h>
#include <unistd.h>

extern "C" {
#include "launcher.h"
}

namespace
{

TEST(LauncherTest, RejectsNullService)
{
    EXPECT_EQ(launcher_start(nullptr, nullptr), -1);
}

TEST(LauncherTest, StopsInactiveService)
{
    Service service{};
    service.pid = -1;
    service.status = SERVICE_STOPPED;

    launcher_stop(&service, nullptr);

    EXPECT_EQ(service.pid, -1);
    EXPECT_EQ(service.status, SERVICE_STOPPED);
}

TEST(LauncherTest, StartsAndStopsExecutable)
{
    Service service{};
    std::snprintf(service.name, SERVICE_NAME_MAX, "%s", "TestService");
    std::snprintf(service.path, SERVICE_PATH_MAX, "%s", "/bin/true");
    service.pid = -1;

    ASSERT_EQ(launcher_start(&service, nullptr), 0);
    EXPECT_GT(service.pid, 0);
    EXPECT_EQ(service.status, SERVICE_RUNNING);

    launcher_stop(&service, nullptr);

    EXPECT_EQ(service.pid, -1);
    EXPECT_EQ(service.status, SERVICE_STOPPED);
}

TEST(LauncherTest, HandlesExecutableFailure)
{
    Service service{};
    std::snprintf(service.name, SERVICE_NAME_MAX, "%s", "MissingService");
    std::snprintf(service.path, SERVICE_PATH_MAX, "%s",
                  "/does/not/exist");

    ASSERT_EQ(launcher_start(&service, nullptr), 0);

    int status = 0;
    ASSERT_EQ(waitpid(service.pid, &status, 0), service.pid);
    EXPECT_TRUE(WIFEXITED(status));
    EXPECT_EQ(WEXITSTATUS(status), 127);
}

TEST(LauncherTest, StopsMissingProcess)
{
    Service service{};
    std::snprintf(service.name, SERVICE_NAME_MAX, "%s", "MissingProcess");
    service.pid = 999999;
    service.status = SERVICE_RUNNING;

    launcher_stop(&service, nullptr);

    EXPECT_EQ(service.pid, -1);
    EXPECT_EQ(service.status, SERVICE_STOPPED);
}

TEST(LauncherTest, StopsAllServices)
{
    Service first{};
    Service second{};
    first.pid = -1;
    first.status = SERVICE_STOPPED;
    second.pid = -1;
    second.status = SERVICE_STOPPED;
    first.next = &second;

    launcher_stop_all(&first, nullptr);

    EXPECT_EQ(first.pid, -1);
    EXPECT_EQ(second.pid, -1);
}

}  // namespace
