#include <gtest/gtest.h>

#include <ctime>
#include <string>

extern "C" {
#include "dashboard.h"
}

TEST(DashboardTest, DisplaysServiceSummary)
{
    Service service{};
    service.name[0] = 'C';
    service.name[1] = 'a';
    service.name[2] = 'm';
    service.name[3] = 'e';
    service.name[4] = 'r';
    service.name[5] = 'a';
    service.name[6] = '\0';
    service.pid = 42;
    service.status = SERVICE_FAILED;
    service.restart_count = 2;

    testing::internal::CaptureStdout();
    dashboard_display(&service, 3);
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Camera"), std::string::npos);
    EXPECT_NE(output.find("FAILED"), std::string::npos);
    EXPECT_NE(output.find("Total Services : 1"), std::string::npos);
    EXPECT_NE(output.find("Total Crashes  : 3"), std::string::npos);
    EXPECT_NE(output.find("Most Unstable  : Camera"), std::string::npos);
}

TEST(DashboardTest, DisplaysRunningAndStoppedServices)
{
    Service running{};
    Service stopped{};
    running.name[0] = 'R';
    running.name[1] = '\0';
    stopped.name[0] = 'S';
    stopped.name[1] = '\0';
    running.pid = 10;
    running.status = SERVICE_RUNNING;
    running.restart_count = 6;
    running.start_time = time(nullptr) - 3661;
    stopped.pid = -1;
    stopped.status = SERVICE_STOPPED;
    stopped.restart_count = 0;
    running.next = &stopped;

    testing::internal::CaptureStdout();
    dashboard_display(&running, 0);
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("RUNNING"), std::string::npos);
    EXPECT_NE(output.find("STOPPED"), std::string::npos);
    EXPECT_NE(output.find("  0%"), std::string::npos);
    EXPECT_NE(output.find("Most Unstable  : R"), std::string::npos);
}

TEST(DashboardTest, DisplaysEmptyServiceList)
{
    testing::internal::CaptureStdout();
    dashboard_display(nullptr, 0);
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_NE(output.find("Total Services : 0"), std::string::npos);
    EXPECT_NE(output.find("Most Unstable  : None"), std::string::npos);
}
