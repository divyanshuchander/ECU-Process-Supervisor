CC := gcc
CXX := g++
CFLAGS := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Iinclude
BUILD_DIR := build
MISRA_BUILD_DIR := build-misra
MISRA_CFLAGS := \
	-Werror \
	-Wconversion \
	-Wsign-conversion \
	-Wshadow \
	-Wstrict-prototypes \
	-Wmissing-prototypes \
	-Wold-style-definition \
	-Wundef \
	-Wswitch-enum \
	-Wcast-qual \
	-Wpointer-arith \
	-Wformat=2 \
	-Wnull-dereference \
	-Wdouble-promotion \
	-Wvla \
	-Wfloat-equal \
	-fanalyzer

SUPERVISOR_SOURCES := \
	src/main.c \
	src/config.c \
	src/launcher.c \
	src/monitor.c \
	src/logger.c \
	src/dashboard.c

SERVICE_NAMES := camera radar lane object diagnostics
SERVICE_TARGETS := $(addsuffix _service,$(SERVICE_NAMES))
SERVICE_SOURCES := $(addprefix services/,$(addsuffix _service.c,$(SERVICE_NAMES)))
SUPERVISOR_OBJECTS := $(SUPERVISOR_SOURCES:%.c=$(BUILD_DIR)/%.o)
GTEST_SOURCES := \
	gtest/config_test.cpp \
	gtest/logger_test.cpp \
	gtest/launcher_test.cpp \
	gtest/dashboard_test.cpp
GTEST_C_SOURCES := src/config.c src/logger.c src/launcher.c src/dashboard.c
GTEST_OBJECTS := $(GTEST_C_SOURCES:src/%.c=$(BUILD_DIR)/gtest/%.o)

.PHONY: all clean misra-compile valgrind-check cppcheck gtest

all: $(BUILD_DIR)/supervisor $(addprefix $(BUILD_DIR)/services/,$(SERVICE_TARGETS))

misra-compile:
	$(MAKE) BUILD_DIR=$(MISRA_BUILD_DIR) CFLAGS="$(CFLAGS) $(MISRA_CFLAGS)" all

cppcheck:
	cppcheck --enable=warning,performance,portability --inconclusive \
		--std=c11 --language=c -Iinclude \
		--suppress=missingIncludeSystem --error-exitcode=1 \
		$(SUPERVISOR_SOURCES) $(SERVICE_SOURCES)

gtest: $(BUILD_DIR)/gtest/tests
	./$(BUILD_DIR)/gtest/tests

$(BUILD_DIR)/gtest/tests: $(GTEST_SOURCES) $(GTEST_OBJECTS) | $(BUILD_DIR)/gtest
	$(CXX) -std=c++17 -Wall -Wextra -Wpedantic -Iinclude $^ \
		-lgtest -lgtest_main -pthread -o $@

$(BUILD_DIR)/gtest/%.o: src/%.c | $(BUILD_DIR)/gtest
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gtest:
	mkdir -p $@

valgrind-check: all
	timeout --signal=INT --kill-after=5s 5s \
		valgrind --trace-children=yes --leak-check=full \
		--show-leak-kinds=all --error-exitcode=1 ./build/supervisor \
		|| test $$? -eq 124

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/supervisor: $(SUPERVISOR_OBJECTS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services/%: $(BUILD_DIR)/services/%.o | $(BUILD_DIR)/services
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services:
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR) $(MISRA_BUILD_DIR)
	$(RM) logs/supervisor.log
