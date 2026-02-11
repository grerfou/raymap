# ============================================================================
# RayMapVid - Makefile Central (pour test_video/)
# ============================================================================

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDES = -I.. -I../src

# Libraries (AVEC FFmpeg)
LIBS = -lraylib -lm -lpthread -ldl -lavformat -lavcodec -lavutil -lswscale

# Build modes
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2 -DNDEBUG
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g

# ============================================================================
# Exemples disponibles
# ============================================================================
#EXAMPLE = 01_compilation
#EXAMPLE = 02_video_loading
#EXAMPLE = 03_video_metadata
#EXAMPLE = 04_texture_access
#EXAMPLE = 05_playback_state
EXAMPLE = 06_frame_decoding

# ============================================================================
# Détection automatique
# ============================================================================
SRC = $(EXAMPLE)/main.c
TARGET = bin/$(EXAMPLE)

# ============================================================================
# Build Targets
# ============================================================================

# Normal build
$(TARGET): $(SRC) ../src/raymapvid.h
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Build successful: $(TARGET)"

# Debug build
debug: ../src/raymapvid.h
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(DEBUG_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Debug build successful: $(TARGET)"

# ASan build
asan: ../src/raymapvid.h
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(ASAN_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ AddressSanitizer build successful: $(TARGET)"

# Release build
release: ../src/raymapvid.h
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(RELEASE_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Release build successful: $(TARGET)"

# ============================================================================
# Execution
# ============================================================================

run: clean $(TARGET)
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║              Running: $(EXAMPLE)"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	./$(TARGET)

run-asan: clean asan
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Running with AddressSanitizer: $(EXAMPLE)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=0 ./$(TARGET)

test-memory: clean asan
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║           MEMORY TEST: $(EXAMPLE)"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@ASAN_OPTIONS=detect_leaks=1:halt_on_error=0 ./$(TARGET)
	@echo ""
	@echo "✓ Memory test complete"

# ============================================================================
# Cleanup
# ============================================================================

clean:
	@rm -rf bin *.log asan.log.*
	@echo "✓ Cleaned build artifacts"

clean-all: clean
	@rm -f core
	@echo "✓ Deep clean complete"

# ============================================================================
# Utilities
# ============================================================================

list:
	@echo "Available examples:"
	@echo ""
	@for dir in */; do \
		if [ -f "$$dir/main.c" ]; then \
			name=$$(basename $$dir); \
			echo "  - $$name"; \
		fi \
	done
	@echo ""
	@echo "Usage: make EXAMPLE=01_compilation run"

info:
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "Build Configuration:"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Compiler:    $(CC)"
	@echo "  Example:     $(EXAMPLE)"
	@echo "  Source:      $(SRC)"
	@echo "  Target:      $(TARGET)"
	@echo "  Flags:       $(CFLAGS)"
	@echo "  Includes:    $(INCLUDES)"
	@echo "  Libraries:   $(LIBS)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

help:
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║                RAYMAPVID BUILD SYSTEM                      ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "BASIC COMMANDS:"
	@echo "  make                          - Build current example"
	@echo "  make run                      - Build and run"
	@echo "  make clean                    - Remove build artifacts"
	@echo ""
	@echo "BUILD MODES:"
	@echo "  make debug                    - Build with debug symbols"
	@echo "  make release                  - Build optimized"
	@echo "  make asan                     - Build with AddressSanitizer"
	@echo ""
	@echo "TESTING:"
	@echo "  make run-asan                 - Run with AddressSanitizer"
	@echo "  make test-memory              - Quick memory test"
	@echo ""
	@echo "EXAMPLES:"
	@echo "  make list                     - List available examples"
	@echo "  make EXAMPLE=01_compilation   - Set example to build"
	@echo ""
	@echo "USAGE:"
	@echo "  make run"
	@echo "  make EXAMPLE=02_video_loading run"
	@echo ""
	@echo "UTILITIES:"
	@echo "  make info                     - Show build configuration"
	@echo "  make help                     - Show this help"
	@echo ""

.PHONY: all debug asan release run run-asan test-memory clean clean-all list info help
