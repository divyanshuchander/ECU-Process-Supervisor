CC := gcc
CFLAGS := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -Iinclude
BUILD_DIR := build

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
SERVICE_OBJECTS := $(SERVICE_SOURCES:%.c=$(BUILD_DIR)/%.o)

.PHONY: all clean

all: $(BUILD_DIR)/supervisor $(addprefix $(BUILD_DIR)/services/,$(SERVICE_TARGETS))

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/supervisor: $(SUPERVISOR_OBJECTS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services/camera_service: $(BUILD_DIR)/services/camera_service.o | $(BUILD_DIR)/services
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services/radar_service: $(BUILD_DIR)/services/radar_service.o | $(BUILD_DIR)/services
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services/lane_service: $(BUILD_DIR)/services/lane_service.o | $(BUILD_DIR)/services
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services/object_service: $(BUILD_DIR)/services/object_service.o | $(BUILD_DIR)/services
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services/diagnostics_service: $(BUILD_DIR)/services/diagnostics_service.o | $(BUILD_DIR)/services
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/services:
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) logs/supervisor.log
