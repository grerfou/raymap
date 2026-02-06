# Makefile pour Test 01_compilation - Étape 1.1

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I..
LIBS = -lraylib -lm

SRC = main.c
TARGET = 01_compilation

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC) ../raymapvid.h
	@echo "Compiling Test 01_compilation..."
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)
	@echo "✓ Compilation successful"

run: $(TARGET)
	@echo ""
	@echo "Running Test 01_compilation..."
	@echo ""
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
	@echo "✓ Cleaned"

help:
	@echo "Makefile pour Test 01_compilation"
	@echo ""
	@echo "Commandes:"
	@echo "  make        - Compile le test"
	@echo "  make run    - Compile et exécute le test"
	@echo "  make clean  - Nettoie les fichiers générés"
