#  RayMap

**Professional projection mapping library for Raylib**

Real-time surface warping, multi-projector calibration, and interactive mapping tools for creative installations, museums, and live events.

[![License](https://img.shields.io/badge/license-zlib-blue.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-99-blue.svg)](https://en.wikipedia.org/wiki/C99)

---

##  Features

-  **Real-time projection mapping** - Bilinear and homography-based warping
-  **Interactive calibration** - Visual corner adjustment with mouse
-  **Multi-surface support** - Handle multiple projectors simultaneously
-  **Single-header library** - Drop `raymap.h` and you're ready
-  **Config persistence** - Save/load calibration settings
-  **GPU-accelerated** - Mesh-based rendering for performance
-  **Production-tested** - Memory-safe, leak-free (Valgrind + AddressSanitizer verified)

---

##  Quick Look

<!-- TODO: Add GIF/screenshot here -->

```c
// Create surface
RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);

// Draw your content
RM_BeginSurface(surface);
    DrawText("Hello Mapping!", 200, 250, 60, WHITE);
RM_EndSurface(surface);

// Display with warping
RM_DrawSurface(surface);
```

---

##  Quick Start

### Installation

1. **Copy the header**
   ```bash
   cp raymap.h your_project/
   ```

2. **Include in ONE C file**
   ```c
   #define RAYMAP_IMPLEMENTATION
   #include "raymap.h"
   ```

3. **Link with raylib**
   ```bash
   gcc main.c -lraylib -lm -o app
   ```

### Minimal Example

```c
#include "raylib.h"

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

int main(void)
{
    InitWindow(1280, 720, "RayMap Example");
    
    // Create mapping surface
    RM_Surface *surface = RM_CreateSurface(640, 480, RM_MAP_BILINEAR);
    
    // Setup calibration
    RM_Calibration calib = RM_CalibrationDefault(surface);
    
    while (!WindowShouldClose())
    {
        // Update calibration (TAB to toggle)
        RM_UpdateCalibrationInput(&calib, KEY_TAB);
        
        // Draw to surface
        RM_BeginSurface(surface);
            ClearBackground(RAYWHITE);
            DrawCircle(320, 240, 100, RED);
        RM_EndSurface(surface);
        
        // Display mapped surface
        BeginDrawing();
            ClearBackground(BLACK);
            RM_DrawSurface(surface);
            RM_DrawCalibration(calib);
        EndDrawing();
    }
    
    RM_DestroySurface(surface);
    CloseWindow();
    return 0;
}
```

---

##  Examples

### Basic Usage
- [`examples/basic/`](examples/basic/) - Simple projection mapping
- [`examples/calibration/`](examples/calibration/) - Interactive calibration

### Advanced
- [`examples/multi_mapping/`](examples/multi_mapping/) - Multiple surfaces simultaneously
  -  **Perfect for multi-projector installations**
  -  Independent content per surface
  -  Per-surface calibration

---

##  Use Cases

| Application        | Example                                          |
|--------------------|--------------------------------------------------|
|  **Event Mapping** | Project on buildings, stages, irregular surfaces |
|  **Game Dev**      | Multi-screen setups, curved displays             |
|  **Museums**       | Interactive exhibitions, architectural mapping   |
|  **Live Shows**    | VJ tools, concert visuals                        |
|  **Corporate**     | Trade show booths, product launches              |

---

##  API Overview

### Core Functions

```c
// Surface management
RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode);
void RM_DestroySurface(RM_Surface *surface);

// Drawing
void RM_BeginSurface(RM_Surface *surface);
void RM_EndSurface(RM_Surface *surface);
void RM_DrawSurface(RM_Surface *surface);

// Calibration
RM_Calibration RM_CalibrationDefault(RM_Surface *surface);
void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey);
void RM_DrawCalibration(RM_Calibration calibration);

// Configuration
bool RM_SaveConfig(const RM_Surface *surface, const char *filepath);
bool RM_LoadConfig(RM_Surface *surface, const char *filepath);

// Quad manipulation
bool RM_SetQuad(RM_Surface *surface, RM_Quad quad);
RM_Quad RM_GetQuad(const RM_Surface *surface);
```

[Full API Documentation →](docs/API.md)

---

##  Architecture

### Mapping Modes

**Bilinear** (`RM_MAP_BILINEAR`)
- Simple 4-point interpolation
- Fast, good for flat surfaces
- Default mesh: 16×16

**Homography** (`RM_MAP_HOMOGRAPHY`)
- Perspective-correct transformation
- Better for tilted/angled surfaces
- Default mesh: 32×32

### Memory Management

- **Zero leaks** - Verified with Valgrind and AddressSanitizer
- **RAII-style cleanup** - Automatic resource management
- **Safe GPU upload** - Validated before swap

---

##  Testing

```bash
cd tests
make test-memory    # Run with AddressSanitizer
```

**Test coverage:**
-  Memory management (16 tests)
-  Surface creation/destruction
-  Mesh generation
-  Quad validation
-  Configuration I/O

---

##  Requirements

- **Raylib 5.0+** ([Download](https://www.raylib.com/))
- **C99 compiler** (GCC, Clang, MSVC)
- **Math library** (`-lm` on Linux/macOS)

### Platform Support

| Platform | Status | Notes             |
|----------|--------|-------------------|
| Linux    | Tested | GCC 9+, Clang 10+ |

---

##  Contributing

Contributions welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Add tests if applicable
4. Submit a pull request

### Code Style

- Follow existing style (K&R-ish, 4 spaces)
- Prefix all public symbols with `RM_`
- Document public API functions
- Keep single-header pattern

---

##  License

**zlib/libpng License**

```
Copyright (c) 2025 grerfou

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
```

---

##  Credits

- Built with [Raylib](https://www.raylib.com/) by Ramon Santamaria
- Inspired by production mapping tools and OpenCV
- Homography math based on DLT algorithm

---

##  Contact & Support

-  **Issues:** [GitHub Issues](https://github.com/grerfou/raymap/issues)
-  **Discussions:** [GitHub Discussions](https://github.com/grerfou/raymap/discussions)
-  **Email:** benoit.fage@icloud.com (for commercial inquiries)

---

##  Showcase

Using RayMap in your project? [Let us know!](https://github.com/grerfou/raymap/discussions)

---

<div align="center">

[📖 Documentation](docs/) | [🎨 Examples](examples/)

</div>
