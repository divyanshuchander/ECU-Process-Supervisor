#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

extern "C" {
#include "config.h"
}

namespace
{

const char *test_config_path = "gtest/test-services.conf";

class ConfigTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::remove(test_config_path);
    }

    void WriteConfig(const std::string &contents)
    {
        std::ofstream config_file(test_config_path);
        config_file << contents;
    }
};

TEST_F(ConfigTest, LoadsServicesAndSkipsComments)
{
    WriteConfig("# comment\n\nCameraService ./camera 3\nRadarService ./radar 1\n");

    Service *services = nullptr;

    ASSERT_EQ(config_load(test_config_path, &services), 0);
    ASSERT_NE(services, nullptr);
    EXPECT_STREQ(services->name, "CameraService");
    EXPECT_EQ(services->max_restart, 3);
    ASSERT_NE(services->next, nullptr);
    EXPECT_STREQ(services->next->name, "RadarService");
    EXPECT_EQ(services->next->max_restart, 1);
    EXPECT_EQ(services->next->next, nullptr);

    config_free(services);
}

TEST_F(ConfigTest, AppendsToExistingServices)
{
    WriteConfig("RadarService ./radar 1\n");
    Service *services = new Service{};
    std::snprintf(services->name, SERVICE_NAME_MAX, "%s", "ExistingService");
    services->next = nullptr;

    ASSERT_EQ(config_load(test_config_path, &services), 0);
    ASSERT_NE(services->next, nullptr);
    EXPECT_STREQ(services->next->name, "RadarService");

    config_free(services);
}

TEST_F(ConfigTest, RejectsInvalidConfiguration)
{
    WriteConfig("CameraService ./camera -1\n");
    Service *services = nullptr;

    EXPECT_EQ(config_load(test_config_path, &services), -1);
    EXPECT_EQ(services, nullptr);
}

TEST_F(ConfigTest, RejectsDuplicateServices)
{
    WriteConfig("CameraService ./camera 3\nCameraService ./other 1\n");
    Service *services = nullptr;

    EXPECT_EQ(config_load(test_config_path, &services), -1);
    EXPECT_EQ(services, nullptr);
}

TEST_F(ConfigTest, RejectsMissingFile)
{
    Service *services = nullptr;

    EXPECT_EQ(config_load("gtest/does-not-exist.conf", &services), -1);
    EXPECT_EQ(services, nullptr);
}

}  // namespace
