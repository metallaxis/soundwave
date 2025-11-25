# ============================================================
#   C Cross-Platform Build System for Aliquo Sequence Project
# ============================================================

# Default Compiler (will be set based on platform)
CC = none

# Default Compiler Flags
CFLAGS = none

# Default Platform (will be detected)
PLATFORM = none

# Debug flag
DEBUG =

# ------------------------------------------------------------
# Linux-specific information
LINUX_CC = gcc
LINUX_CFLAGS = -Ofast -Wall -Wextra -Werror -pedantic 
LINUX_LDFLAGS = -lm

# Windows-specific information
WINDOWS_CC = x86_64-w64-mingw32-gcc
WINDOWS_CFLAGS = -Ofast -Wall -Wextra -Werror -pedantic
WINDOWS_LDFLAGS = -lm

# ------------------------------------------------------------
# Project folders
SRC = ./src
BUILD_DIR = ./build

# ------------------------------------------------------------
# Detect platform based on make command arguments
ifneq (,$(findstring linux,$(MAKECMDGOALS)))
PLATFORM := linux
endif

ifneq (,$(findstring windows,$(MAKECMDGOALS)))
PLATFORM := windows
endif

# ------------------------------------------------------------
# Project source files
# Recursively find all .c files under src/
CODE = $(shell find $(SRC) -name '*.c')
OBJS = $(CODE) $(RESOURCES_C)

# ------------------------------------------------------------
# Output binary name
ifeq ($(PLATFORM), windows)
	CC = $(WINDOWS_CC)
	CFLAGS = $(WINDOWS_CFLAGS)
	OUT = $(BUILD_DIR)/soundwave.exe
	LDFLAGS = $(WINDOWS_LDFLAGS)
else ifeq ($(PLATFORM), linux)
	CC = $(LINUX_CC)
	CFLAGS = $(LINUX_CFLAGS)
	OUT = $(BUILD_DIR)/soundwave
	LDFLAGS = $(LINUX_LDFLAGS)
endif

# ------------------------------------------------------------
# Debug flag
ifneq (,$(findstring --debug,$(MAKECMDGOALS)))
	DEBUG = -g
	CFLAGS += $(DEBUG)
endif

# ------------------------------------------------------------
# Default target: Display usage help
default:
	@echo "Usage:"
	@echo "  make build linux|windows           - Build for your platform"
	@echo "  make run linux|windows             - Run the built program"
	@echo "  make build --debug linux|windows   - Build with debug symbols"
	@echo "  make run --debug linux|windows     - Run with debug symbols"
	@echo "  make clean                         - Clean the build directory"

# ------------------------------------------------------------
# Build target
$(OUT): $(OBJS)
	@echo "Building for platform: $(PLATFORM)"
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT) $(LDFLAGS)

# ------------------------------------------------------------
# Run target
run: $(OUT)
ifeq ($(PLATFORM), windows)
	@echo "Running application..."
	@start "" "$(OUT)"
else ifeq ($(PLATFORM), linux)
	@echo "Running application..."
	$(OUT)
else
	@echo "Specify a platform: make run linux or make run windows"
endif

# ------------------------------------------------------------
# Clean target
clean:
	rm -rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)

# ------------------------------------------------------------
# Build target for platform
build: clean
ifeq ($(PLATFORM), none)
	@echo "Specify a platform: make build linux or make build windows"
else
	make $(OUT) $(PLATFORM)
endif

# ------------------------------------------------------------
# Declare linux/windows as fake targets so make doesn’t fail
.PHONY: linux windows
linux:
	@true
windows:
	@true
