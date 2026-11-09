#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

extern "C" {
#include "logger.h"
}

namespace
{

const char *log_path = "gtest/test.log";

class LoggerTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::remove(log_path);
    }
};

TEST_F(LoggerTest, OpensWritesAndClosesLog)
{
    FILE *log_file = nullptr;

    ASSERT_EQ(logger_open(&log_file, log_path), 0);
    ASSERT_NE(log_file, nullptr);
    EXPECT_EQ(logger_log(log_file, "Service %s exited %d", "Camera", 0), 0);
    logger_close(log_file);

    std::ifstream output(log_path);
    const std::string contents((std::istreambuf_iterator<char>(output)),
                               std::istreambuf_iterator<char>());
    EXPECT_NE(contents.find("Service Camera exited 0"), std::string::npos);
}

TEST_F(LoggerTest, RejectsNullArguments)
{
    FILE *log_file = nullptr;

    EXPECT_EQ(logger_open(nullptr, "gtest/test.log"), -1);
    EXPECT_EQ(logger_open(&log_file, nullptr), -1);
    EXPECT_EQ(logger_log(nullptr, "message"), -1);
    EXPECT_EQ(logger_log(stdout, nullptr), -1);
}

}  // namespace
