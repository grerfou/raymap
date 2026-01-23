# Compiler
CC = gcc
CFLAGS =  -Wall -Wextra -std=c99
INCLUDES = -I./../../src
LIBS = -lraylib -lm -lpthread -ldl

# Exemple à compiler

#EXAMPLE = 01_compilation
EXAMPLE = 02_surface_lifecycle
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


# Future commit --> git commity -m "fix: Replace printf with TraceLog, add RM_IsQuadValid + refactor: cleaning and program commentary"


SRC = $(EXAMPLE)/main.c
TARGET = bin/$(EXAMPLE)

all: $(TARGET)

$(TARGET):
	@mkdir -p bin
	$(CC) $(SRC) $(INCLUDES) $(CFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -rf bin *.txt

run : clean $(TARGET)
	./$(TARGET)


.PHONY : all clean run
