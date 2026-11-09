#include <gtest/gtest.h>

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
