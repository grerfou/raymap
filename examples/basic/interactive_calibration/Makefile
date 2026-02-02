# RayMap Example 02 - Interactive Calibration
# Makefile

# Compiler
CC = gcc

# Source files
SRC = main.c

# Output binary
TARGET = interactive_calibration

# Include path
RAYMAP_INCLUDE = ../../../src

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -I$(RAYMAP_INCLUDE)

# Libraries
LIBS = -lraylib -lm

# Platform-specific libraries
ifeq ($(OS),Windows_NT)
    LIBS += -lopengl32 -lgdi32 -lwinmm
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LIBS += -lpthread -ldl -lrt
    endif
    ifeq ($(UNAME_S),Darwin)
        LIBS += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
    endif
endif

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRC)
	@echo "Building $(TARGET)..."
	$(CC) $(SRC) $(CFLAGS) $(LIBS) -o $(TARGET)
	@echo "✓ Build successful!"

# Run the example
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts and config files
clean:
	@echo "Cleaning..."
	rm -f $(TARGET)
	@echo "✓ Clean complete!"

# Clean only binary (keep calibration.txt)
clean-bin:
	@echo "Cleaning binary..."
	rm -f $(TARGET)
	@echo "✓ Binary removed!"

# Help
help:
	@echo "RayMap Example 02 - Interactive Calibration"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build the example"
	@echo "  make run      - Build and run the example"
	@echo "  make clean    - Remove all build artifacts and config"
	@echo "  make clean-bin - Remove only binary (keep calibration.txt)"
	@echo "  make help     - Show this help"

.PHONY: all run clean clean-bin help
