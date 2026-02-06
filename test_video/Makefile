# ============================================================================
# RayMapVid - Makefile
# ============================================================================

# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDES = -I../src

# FFmpeg detection (system)
FFMPEG_CFLAGS = $(shell pkg-config --cflags libavcodec libavformat libavutil libswscale 2>/dev/null || echo "")
FFMPEG_LIBS = $(shell pkg-config --libs libavcodec libavformat libavutil libswscale 2>/dev/null || echo "-lavcodec -lavformat -lavutil -lavswscale")

INCLUDES += $(FFMPEG_CFLAGS)
LIBS = -lraylib -lm -lpthread -ldl $(FFMPEG_LIBS)

# Build modes
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2 -DNDEBUG
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g

# Source and target
#SRC = test_simple/main.c
SRC = 01_compilation/main.c
TARGET = bin/raymapvid

# ============================================================================
# Quick Commands
# ============================================================================
# make              - Build
# make run          - Build and run with Valgrind + ASan
# make test-memory  - Quick ASan test
# make test-valgrind- Full Valgrind test
# make clean        - Clean artifacts
# make help         - Show help
# ============================================================================

# Default target
all: $(TARGET)

# ============================================================================
# Build Targets
# ============================================================================

# Normal build
$(TARGET):
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Build successful: $(TARGET)"

# Debug build (with symbols)
debug:
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(DEBUG_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Debug build successful: $(TARGET)"

# AddressSanitizer build
asan:
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(ASAN_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ AddressSanitizer build successful: $(TARGET)"
	@echo "ℹ Run with: make run-asan"

# Release build (optimized)
release:
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(RELEASE_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Release build successful: $(TARGET)"

# ============================================================================
# Execution Targets
# ============================================================================

# Normal run
run: clean
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║              RAYMAPVID - FULL TEST SUITE                   ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "━━━ Phase 1: Valgrind Test ━━━"
	@$(MAKE) -s run-valgrind-quiet
	@echo ""
	@echo "━━━ Phase 2: AddressSanitizer Test ━━━"
	@$(MAKE) -s run-asan-quiet
	@echo ""
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║                   ✓ ALL TESTS PASSED                       ║"
	@echo "╚════════════════════════════════════════════════════════════╝"

# Run with AddressSanitizer
run-asan: clean asan
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Running with AddressSanitizer"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=0:log_path=asan.log ./$(TARGET)
	@echo ""
	@if [ -f asan.log.* ]; then \
		echo "⚠ AddressSanitizer detected issues. Check asan.log.*"; \
		cat asan.log.*; \
		exit 1; \
	else \
		echo "✓ No memory issues detected!"; \
	fi

# Run with Valgrind
run-valgrind: clean debug
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Running with Valgrind"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@which valgrind > /dev/null || (echo "❌ Valgrind not installed. Install with: sudo apt install valgrind" && exit 1)
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --verbose \
	         --log-file=valgrind.log \
	         ./$(TARGET)
	@echo ""
	@echo "📄 Full report saved to: valgrind.log"
	@echo ""
	@echo "━━━ Valgrind Summary ━━━"
	@grep -A 5 "LEAK SUMMARY" valgrind.log || echo "No leaks detected!"
	@grep "All heap blocks were freed" valgrind.log && echo "✓ Perfect: All memory freed!" || echo "⚠ Check valgrind.log for details"

# ============================================================================
# Memory Testing (Internal - for 'make run')
# ============================================================================

run-asan-quiet: asan
	@ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 ./$(TARGET) > /dev/null 2>&1 && \
		echo "✓ AddressSanitizer: PASS" || \
		(echo "✗ AddressSanitizer: FAIL" && exit 1)

run-valgrind-quiet: debug
	@which valgrind > /dev/null || (echo "⚠ Valgrind not found (skipping)" && exit 0)
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --error-exitcode=1 \
	          --log-file=valgrind_quiet.log \
	          ./$(TARGET) > /dev/null 2>&1 && \
		echo "✓ Valgrind: PASS (no leaks)" || \
		(echo "✗ Valgrind: FAIL (check valgrind_quiet.log)" && exit 1)

# ============================================================================
# Detailed Testing
# ============================================================================

# Quick memory test (AddressSanitizer only)
test-memory: clean asan
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║           RAYMAPVID MEMORY TESTS - ASAN ONLY              ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Running AddressSanitizer tests..."
	@ASAN_OPTIONS=detect_leaks=1:halt_on_error=0 ./$(TARGET)
	@echo ""
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "Memory test complete. Checking for leaks..."
	@if [ -f asan.log.* ]; then \
		echo "⚠ Issues found. Review asan.log.*"; \
		exit 1; \
	else \
		echo "✓ All memory tests passed!"; \
	fi

# Full Valgrind test
test-valgrind: clean debug
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║           RAYMAPVID MEMORY TESTS - VALGRIND               ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@which valgrind > /dev/null || (echo "❌ Install valgrind first: sudo apt install valgrind" && exit 1)
	@echo ""
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --error-exitcode=1 \
	          ./$(TARGET) 2>&1 | tee valgrind_test.log
	@echo ""
	@grep -q "All heap blocks were freed" valgrind_test.log && \
		echo "✓ Valgrind: All memory freed!" || \
		(echo "❌ Memory leaks detected! Check valgrind_test.log" && exit 1)

# ============================================================================
# Cleanup
# ============================================================================

clean:
	rm -rf bin *.txt asan.log.* valgrind.log valgrind_test.log valgrind_quiet.log
	@echo "✓ Cleaned build artifacts"

clean-all: clean
	rm -f *.log core
	@echo "✓ Deep clean complete"

# ============================================================================
# Utilities
# ============================================================================

# Show current configuration
info:
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "Build Configuration:"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Compiler:      $(CC)"
	@echo "  Source:        $(SRC)"
	@echo "  Target:        $(TARGET)"
	@echo "  Flags:         $(CFLAGS)"
	@echo "  Includes:      $(INCLUDES)"
	@echo "  Libraries:     $(LIBS)"
	@echo "  FFmpeg Flags:  $(FFMPEG_CFLAGS)"
	@echo "  FFmpeg Libs:   $(FFMPEG_LIBS)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@echo ""
	@which gcc > /dev/null && echo "✓ gcc found" || echo "✗ gcc not found"
	@which valgrind > /dev/null && echo "✓ valgrind found" || echo "⚠ valgrind not found (optional)"
	@pkg-config --exists raylib && echo "✓ raylib found" || echo "✗ raylib not found"
	@pkg-config --exists libavcodec && echo "✓ FFmpeg found" || echo "✗ FFmpeg not found"
	@echo ""
	@echo "Install missing dependencies:"
	@echo "  Ubuntu: sudo apt install gcc valgrind libraylib-dev libavcodec-dev libavformat-dev libavutil-dev libswscale-dev"
	@echo "  macOS:  brew install gcc raylib ffmpeg"

# Help
help:
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║                RAYMAPVID BUILD SYSTEM                      ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "BASIC COMMANDS:"
	@echo "  make              - Build executable"
	@echo "  make run          - Run FULL test suite (Valgrind + ASan)"
	@echo "  make clean        - Remove build artifacts"
	@echo ""
	@echo "BUILD MODES:"
	@echo "  make debug        - Build with debug symbols (-g -O0)"
	@echo "  make release      - Build optimized (-O2)"
	@echo "  make asan         - Build with AddressSanitizer"
	@echo ""
	@echo "MEMORY TESTING:"
	@echo "  make test-memory  - Quick ASan memory test"
	@echo "  make test-valgrind- Full Valgrind test suite"
	@echo "  make run-asan     - Run with AddressSanitizer"
	@echo "  make run-valgrind - Run with Valgrind"
	@echo ""
	@echo "UTILITIES:"
	@echo "  make info         - Show build configuration"
	@echo "  make check-deps   - Check dependencies"
	@echo "  make help         - Show this help"
	@echo ""
	@echo "EXAMPLES:"
	@echo "  make run          # Full test suite"
	@echo "  make test-memory  # Quick memory check"
	@echo ""

# Phony targets
.PHONY: all debug asan release run run-asan run-valgrind run-asan-quiet run-valgrind-quiet \
        test-memory test-valgrind clean clean-all info check-deps help
