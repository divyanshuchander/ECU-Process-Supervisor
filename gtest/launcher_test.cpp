#include <gtest/gtest.h>

#include <cstdio>

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

}  // namespace
