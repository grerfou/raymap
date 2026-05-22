#**************************************************************************************************
#
#   raymap examples/video - Makefile
#
#   Copyright (c) 2025 grerfou
#
#   This software is provided "as-is", without any express or implied warranty.
#
#**************************************************************************************************

.PHONY: all clean clear help debug 09_basic_video_playback 10_video_projection_mapping 11_video_effects 12_multi_video_zones

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I../../src

# Raylib + FFmpeg settings (adjust these paths if needed)
# On Linux with system libraries:
LDFLAGS = -lraylib -lavcodec -lavformat -lavutil -lswscale -lm
# On macOS:
# LDFLAGS = -lraylib -lavcodec -lavformat -lavutil -lswscale -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
# On Windows (MinGW):
# LDFLAGS = -lraylib -lavcodec -lavformat -lavutil -lswscale -lopengl32 -lgdi32 -lwinmm

# Debug/Release flags
DEBUG_FLAGS   = -g -O0 -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Default to release build
CFLAGS += $(RELEASE_FLAGS)

# Examples list
EXAMPLES = 09_basic_video_playback \
           10_video_projection_mapping \
           11_video_effects \
           12_multi_video_zones \

# Output directory
BUILD_DIR      = ../../build/examples/video
RAYMAP_HEADER  = ../../src/raymap.h
RMVID_HEADER   = ../../src/raymapvid.h

#--------------------------------------------------------------------------------------------
# Targets
#--------------------------------------------------------------------------------------------

all: $(BUILD_DIR) $(EXAMPLES)
	@echo "======================================"
	@echo "RayMap Video Examples - Build Complete"
	@echo "======================================"
	@echo "Executables are in: $(BUILD_DIR)/"
	@echo ""
	@echo "To run examples:"
	@echo "  cd $(BUILD_DIR) && ./09_basic_video_playback"
	@echo ""

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "Created build directory: $(BUILD_DIR)"

# Build individual examples
09_basic_video_playback: $(BUILD_DIR)/09_basic_video_playback

$(BUILD_DIR)/09_basic_video_playback: 09_basic_video_playback.c $(RAYMAP_HEADER) $(RMVID_HEADER) | $(BUILD_DIR)
	@echo "Compiling 09_basic_video_playback..."
	@$(CC) $(CFLAGS) 09_basic_video_playback.c -o $(BUILD_DIR)/09_basic_video_playback $(LDFLAGS)

10_video_projection_mapping: $(BUILD_DIR)/10_video_projection_mapping

$(BUILD_DIR)/10_video_projection_mapping: 10_video_projection_mapping.c $(RAYMAP_HEADER) $(RMVID_HEADER) | $(BUILD_DIR)
	@echo "Compiling 10_video_projection_mapping..."
	@$(CC) $(CFLAGS) 10_video_projection_mapping.c -o $(BUILD_DIR)/10_video_projection_mapping $(LDFLAGS)

11_video_effects: $(BUILD_DIR)/11_video_effects

$(BUILD_DIR)/11_video_effects: 11_video_effects.c $(RAYMAP_HEADER) $(RMVID_HEADER) | $(BUILD_DIR)
	@echo "Compiling 11_video_effects..."
	@$(CC) $(CFLAGS) 11_video_effects.c -o $(BUILD_DIR)/11_video_effects $(LDFLAGS)

12_multi_video_zones: $(BUILD_DIR)/12_multi_video_zones

$(BUILD_DIR)/12_multi_video_zones: 12_multi_video_zones.c $(RAYMAP_HEADER) $(RMVID_HEADER) | $(BUILD_DIR)
	@echo "Compiling 12_multi_video_zones..."
	@$(CC) $(CFLAGS) 12_multi_video_zones.c -o $(BUILD_DIR)/12_multi_video_zones $(LDFLAGS)

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
	@echo "RayMap Video Examples - Makefile Help"
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build all examples (release mode)"
	@echo "  make debug        - Build all examples (debug mode)"
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Individual examples:"
	@echo "  make 09_basic_video_playback"
	@echo "  make 10_video_projection_mapping"
	@echo "  make 11_video_effects"
	@echo "  make 12_multi_video_zones"
	@echo ""
	@echo "Requirements:"
	@echo "  raylib 5.0+, FFmpeg 4.4+"
	@echo "  Linux: sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev"
	@echo ""
	@echo "Platform-specific compilation:"
	@echo "  Linux:   Uses system libraries (-lraylib -lavcodec ...)"
	@echo "  macOS:   Edit LDFLAGS to add -framework flags"
	@echo "  Windows: Edit LDFLAGS for MinGW (-lopengl32 -lgdi32 -lwinmm)"
	@echo ""
	@echo "Example usage:"
	@echo "  make                               # Build all"
	@echo "  make 12_multi_video_zones          # Build single example"
	@echo "  cd ../../build/examples/video      # Navigate to output"
	@echo "  ./12_multi_video_zones             # Run example"
	@echo ""
