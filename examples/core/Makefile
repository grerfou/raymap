#**************************************************************************************************
#
#   raymap examples/core - Makefile
#
#   Copyright (c) 2025 grerfou
#
#   This software is provided "as-is", without any express or implied warranty.
#
#**************************************************************************************************

.PHONY: all clean clear help debug 01_minimal_surface 02_basic_warping 03_interactive_calibration 04_mesh_resolution 05_point_mapping

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I../../src

# Raylib settings (adjust these paths if needed)
# On Linux with system raylib:
LDFLAGS = -lraylib -lm

# On macOS:
# LDFLAGS = -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

# On Windows (MinGW):
# LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Debug/Release flags
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Default to release build
CFLAGS += $(RELEASE_FLAGS)

# Examples list
EXAMPLES = 01_minimal_surface \
           02_basic_warping \
           03_interactive_calibration \
           04_mesh_resolution \
           05_point_mapping

# Output directory
BUILD_DIR = ../../build/examples/core
RAYMAP_HEADER = ../../src/raymap.h

#--------------------------------------------------------------------------------------------
# Targets
#--------------------------------------------------------------------------------------------

all: $(BUILD_DIR) $(EXAMPLES)
	@echo "======================================"
	@echo "RayMap Core Examples - Build Complete"
	@echo "======================================"
	@echo "Executables are in: $(BUILD_DIR)/"
	@echo ""
	@echo "To run examples:"
	@echo "  cd $(BUILD_DIR) && ./01_minimal_surface"
	@echo ""

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "Created build directory: $(BUILD_DIR)"

# Build individual examples
01_minimal_surface: $(BUILD_DIR)/01_minimal_surface

$(BUILD_DIR)/01_minimal_surface: 01_minimal_surface.c $(RAYMAP_HEADER) | $(BUILD_DIR)
	@echo "Compiling 01_minimal_surface..."
	@$(CC) $(CFLAGS) 01_minimal_surface.c -o $(BUILD_DIR)/01_minimal_surface $(LDFLAGS)

02_basic_warping: $(BUILD_DIR)/02_basic_warping

$(BUILD_DIR)/02_basic_warping: 02_basic_warping.c $(RAYMAP_HEADER) | $(BUILD_DIR)
	@echo "Compiling 02_basic_warping..."
	@$(CC) $(CFLAGS) 02_basic_warping.c -o $(BUILD_DIR)/02_basic_warping $(LDFLAGS)

03_interactive_calibration: $(BUILD_DIR)/03_interactive_calibration

$(BUILD_DIR)/03_interactive_calibration: 03_interactive_calibration.c $(RAYMAP_HEADER) | $(BUILD_DIR)
	@echo "Compiling 03_interactive_calibration..."
	@$(CC) $(CFLAGS) 03_interactive_calibration.c -o $(BUILD_DIR)/03_interactive_calibration $(LDFLAGS)

04_mesh_resolution: $(BUILD_DIR)/04_mesh_resolution

$(BUILD_DIR)/04_mesh_resolution: 04_mesh_resolution.c $(RAYMAP_HEADER) | $(BUILD_DIR)
	@echo "Compiling 04_mesh_resolution..."
	@$(CC) $(CFLAGS) 04_mesh_resolution.c -o $(BUILD_DIR)/04_mesh_resolution $(LDFLAGS)

05_point_mapping: $(BUILD_DIR)/05_point_mapping

$(BUILD_DIR)/05_point_mapping: 05_point_mapping.c $(RAYMAP_HEADER) | $(BUILD_DIR)
	@echo "Compiling 05_point_mapping..."
	@$(CC) $(CFLAGS) 05_point_mapping.c -o $(BUILD_DIR)/05_point_mapping $(LDFLAGS)

#--------------------------------------------------------------------------------------------
# Debug build
#--------------------------------------------------------------------------------------------

debug: CFLAGS = -Wall -Wextra -std=c99 -I../../src $(DEBUG_FLAGS)
debug: clean all
	@echo "Built in DEBUG mode"

#--------------------------------------------------------------------------------------------
# Cleanup
#--------------------------------------------------------------------------------------------

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Alias pour clean
clear: clean

#--------------------------------------------------------------------------------------------
# Help
#--------------------------------------------------------------------------------------------

help:
	@echo "RayMap Core Examples - Makefile Help"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build all examples (release mode)"
	@echo "  make debug        - Build all examples (debug mode)"
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Individual examples:"
	@echo "  make 01_minimal_surface"
	@echo "  make 02_basic_warping"
	@echo "  make 03_interactive_calibration"
	@echo "  make 04_mesh_resolution"
	@echo "  make 05_point_mapping"
	@echo ""
	@echo "Platform-specific compilation:"
	@echo "  Linux:   Uses system raylib (-lraylib -lm)"
	@echo "  macOS:   Edit LDFLAGS to add -framework flags"
	@echo "  Windows: Edit LDFLAGS for MinGW (-lopengl32 -lgdi32 -lwinmm)"
	@echo ""
	@echo "Example usage:"
	@echo "  make                          # Build all"
	@echo "  make 01_minimal_surface       # Build single example"
	@echo "  cd ../../build/examples/core  # Navigate to output"
	@echo "  ./01_minimal_surface          # Run example"
	@echo ""
