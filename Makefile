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

all: $(BUILD_DIR)/supervisor $(addprefix $(BUILD_DIR)/,$(SERVICE_TARGETS))

$(BUILD_DIR)/supervisor: $(SUPERVISOR_OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/camera_service: $(BUILD_DIR)/services/camera_service.o
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/radar_service: $(BUILD_DIR)/services/radar_service.o
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/lane_service: $(BUILD_DIR)/services/lane_service.o
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/object_service: $(BUILD_DIR)/services/object_service.o
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/diagnostics_service: $(BUILD_DIR)/services/diagnostics_service.o
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(SUPERVISOR_OBJECTS:.o=.d) $(SERVICE_OBJECTS:.o=.d)

clean:
	$(RM) -r $(BUILD_DIR)
