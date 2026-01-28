# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDES = -I./../../src
LIBS = -lraylib -lm -lpthread -ldl

# Build modes
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2 -DNDEBUG
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -g

# make test-memory (test complet AdressSinitizer rapide)
# make run-asan (executer avec détection fuites)
# make test-valgrind (test Valgrind complet precis)
# make help (aide complete)


# Exemples à compiler
#EXAMPLE = 01_compilation
#EXAMPLE = 02_surface_lifecycle
#EXAMPLE = 03_simple_render
#EXAMPLE = 04_quad_manipulation
#EXAMPLE = 05_mesh_subdivision
#EXAMPLE = 06_mesh_resolution
#EXAMPLE = 07_warp_modes
#EXAMPLE = 08_calibration_input
#EXAMPLE = 09_calibration_ui
#EXAMPLE = 10_calibration_utils
#EXAMPLE = 11_homography_math
#EXAMPLE = 12_perspective_mode
#EXAMPLE = 13_mode_comparaison
#EXAMPLE = 14_config_io
#EXAMPLE = 14_test
#EXAMPLE = 16_geometry_utils
#EXAMPLE = 17_point_mapping
#EXAMPLE = test
EXAMPLE = memory_test

SRC = $(EXAMPLE)/main.c
TARGET = bin/$(EXAMPLE)

# Détection automatique du test mémoire
ifeq ($(EXAMPLE),memory_test)
    # Pour memory_test, utiliser test_memory.c à la racine si main.c n'existe pas
    ifeq ($(wildcard $(EXAMPLE)/main.c),)
        SRC = test_memory.c
    endif
endif

# Targets par défaut
all: $(TARGET)

# Build normal
$(TARGET):
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Build successful: $(TARGET)"

# Build avec debug symbols
debug:
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(DEBUG_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Debug build successful: $(TARGET)"

# Build avec AddressSanitizer (détection fuites mémoire)
asan:
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(ASAN_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ AddressSanitizer build successful: $(TARGET)"
	@echo "ℹ Run with: make run-asan"

# Build release optimisé
release:
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(RELEASE_FLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Release build successful: $(TARGET)"

# Exécution normale
run: clean $(TARGET)
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Running: $(EXAMPLE)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	./$(TARGET)

# Exécution avec AddressSanitizer
run-asan: clean asan
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Running with AddressSanitizer: $(EXAMPLE)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	ASAN_OPTIONS=detect_leaks=1:halt_on_error=0:log_path=asan.log ./$(TARGET)
	@if [ -f asan.log.* ]; then \
		echo "\n⚠ AddressSanitizer detected issues. Check asan.log.*"; \
	else \
		echo "\n✓ No memory issues detected!"; \
	fi

# Exécution avec Valgrind (Linux uniquement)
run-valgrind: clean debug
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Running with Valgrind: $(EXAMPLE)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@which valgrind > /dev/null || (echo "❌ Valgrind not installed. Install with: sudo apt-get install valgrind" && exit 1)
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --verbose \
	         --log-file=valgrind.log \
	         ./$(TARGET)
	@echo "\n📄 Full report saved to: valgrind.log"
	@echo "\n━━━ Valgrind Summary ━━━"
	@grep -A 5 "LEAK SUMMARY" valgrind.log || echo "No leaks detected!"
	@grep "All heap blocks were freed" valgrind.log && echo "✓ Perfect: All memory freed!" || true

# Tests mémoire complets
test-memory: clean asan
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║           RAYMAP MEMORY TESTS - FULL SUITE                 ║"
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

# Tests Valgrind (alternative)
test-valgrind: clean debug
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║           RAYMAP MEMORY TESTS - VALGRIND                   ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@which valgrind > /dev/null || (echo "❌ Install valgrind first" && exit 1)
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --error-exitcode=1 \
	          ./$(TARGET) 2>&1 | tee valgrind_test.log
	@grep -q "All heap blocks were freed" valgrind_test.log && \
		echo "✓ Valgrind: All memory freed!" || \
		(echo "❌ Memory leaks detected!" && exit 1)

# Nettoyage
clean:
	rm -rf bin *.txt asan.log.* valgrind.log valgrind_test.log
	@echo "✓ Cleaned build artifacts"

# Nettoyage complet (inclus les logs)
clean-all: clean
	rm -f *.log core
	@echo "✓ Deep clean complete"

# Aide
help:
	@echo "╔════════════════════════════════════════════════════════════╗"
	@echo "║                  RAYMAP BUILD SYSTEM                       ║"
	@echo "╚════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "BASIC COMMANDS:"
	@echo "  make              - Build current example"
	@echo "  make run          - Clean, build and run"
	@echo "  make clean        - Remove build artifacts"
	@echo ""
	@echo "BUILD MODES:"
	@echo "  make debug        - Build with debug symbols (-g -O0)"
	@echo "  make release      - Build optimized (-O2)"
	@echo "  make asan         - Build with AddressSanitizer"
	@echo ""
	@echo "MEMORY TESTING:"
	@echo "  make test-memory  - Run memory tests with AddressSanitizer"
	@echo "  make run-asan     - Run with AddressSanitizer"
	@echo "  make run-valgrind - Run with Valgrind (Linux)"
	@echo "  make test-valgrind- Full Valgrind test suite"
	@echo ""
	@echo "USAGE:"
	@echo "  1. Edit EXAMPLE variable in Makefile"
	@echo "  2. Run: make test-memory"
	@echo "  3. Check for memory leaks"
	@echo ""
	@echo "EXAMPLE for memory_test:"
	@echo "  EXAMPLE = memory_test"
	@echo "  make test-memory"
	@echo ""

# Info sur la configuration actuelle
info:
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "Build Configuration:"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  Compiler:    $(CC)"
	@echo "  Example:     $(EXAMPLE)"
	@echo "  Source:      $(SRC)"
	@echo "  Target:      $(TARGET)"
	@echo "  Flags:       $(CFLAGS)"
	@echo "  Includes:    $(INCLUDES)"
	@echo "  Libraries:   $(LIBS)"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

.PHONY: all debug asan release run run-asan run-valgrind test-memory test-valgrind clean clean-all help info
