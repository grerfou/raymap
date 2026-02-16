# RayMap - Core Examples

This directory contains the **essential core examples** for getting started with RayMap. These examples demonstrate the fundamental features and usage patterns of the library.

##  Examples Overview

### 01_minimal_surface.c
**The absolute minimum** - "Hello World" of RayMap

**What it demonstrates:**
- Creating a surface
- Drawing content to a surface
- Rendering the surface to screen

**Run:** `./01_minimal_surface`

---

### 02_basic_warping.c
**Warping demonstration** - See the difference between mapping modes

**What it demonstrates:**
- Setting a custom quad (trapezoid shape)
- Switching between BILINEAR and HOMOGRAPHY modes
- Understanding the visual difference

**Key features:**
- Press `SPACE` to toggle between modes
- Grid overlay shows distortion
- Colored circles highlight warping effects

**Run:** `./02_basic_warping`

---

### 03_interactive_calibration.c
**Complete calibration workflow** - The most important example!

**What it demonstrates:**
- Interactive corner dragging
- Calibration mode toggle
- Save/Load configuration to file
- Visual overlay (corners, grid, border)

**Key features:**
- `C` - Toggle calibration on/off
- `R` - Reset quad to default
- `S` - Save configuration
- `L` - Load configuration
- Drag corners with mouse

**Use case:** This is the example users will reference most for real-world projection mapping setups.

**Run:** `./03_interactive_calibration`

---

### 04_mesh_resolution.c
**Performance vs Quality trade-off**

**What it demonstrates:**
- Impact of mesh resolution on warping quality
- Real-time resolution adjustment
- Performance implications

**Key features:**
- `UP` - Increase resolution
- `DOWN` - Decrease resolution
- `SPACE` - Reset to default
- Complex curved patterns stress-test the mesh
- Live vertex/triangle count display

**Use case:** Understanding when to use high vs low mesh resolution.

**Run:** `./04_mesh_resolution`

---

### 05_point_mapping.c
**Interactive click detection** on warped surfaces

**What it demonstrates:**
- `RM_UnmapPoint()` - Screen → Texture coordinates
- `RM_MapPoint()` - Texture → Screen coordinates
- Click detection on distorted surfaces

**Key features:**
- `LEFT CLICK` - Add marker at mouse position
- `RIGHT CLICK` - Clear all markers
- `C` - Toggle calibration
- Markers stay in place when surface is warped!

**Use case:** Interactive installations, touch screens, click detection.

**Run:** `./05_point_mapping`

---

##  Building

### Quick Start (Linux)
```bash
cd examples/core
make
cd ../../build/examples/core
./01_minimal_surface
```

### Platform-Specific

#### Linux
```bash
make
```

#### macOS
Edit `Makefile`, uncomment macOS `LDFLAGS`, then:
```bash
make
```

#### Windows (MinGW)
Edit `Makefile`, uncomment Windows `LDFLAGS`, then:
```bash
mingw32-make
```

### Debug Build
```bash
make debug
```

### Build Single Example
```bash
make 01_minimal_surface
```

### Clean
```bash
make clean
```

---

##  Learning Path

**Recommended order:**

1. **01_minimal_surface.c** - Understand basic workflow (5 min)
2. **02_basic_warping.c** - See warping in action (10 min)
3. **03_interactive_calibration.c** - Learn the full calibration workflow (20 min)
4. **04_mesh_resolution.c** - Understand performance trade-offs (10 min)
5. **05_point_mapping.c** - Master coordinate transformations (15 min)

**Total learning time: ~1 hour**

After these 5 examples, you'll understand:
-  How to create and render surfaces
-  How warping works (bilinear vs homography)
-  How to calibrate for real-world projection
-  How to optimize mesh resolution
-  How to handle user interaction on warped surfaces

---

##  Common Patterns

### Basic Surface Setup
```c
// Create
RM_Surface *surface = RM_CreateSurface(800, 600, RM_MAP_HOMOGRAPHY);

// Draw to it
RM_BeginSurface(surface);
    DrawText("Content", 10, 10, 20, WHITE);
RM_EndSurface(surface);

// Render it
BeginDrawing();
    RM_DrawSurface(surface);
EndDrawing();

// Cleanup
RM_DestroySurface(surface);
```

### Calibration Setup
```c
// Initialize
RM_Calibration calib = RM_CalibrationDefault(surface);
calib.enabled = true;

// Update (in main loop)
RM_UpdateCalibration(&calib);

// Draw overlay
RM_DrawCalibration(calib);

// Save/Load
RM_SaveConfig(surface, "config.cfg");
RM_LoadConfig(surface, "config.cfg");
```

### Point Mapping
```c
// Screen → Texture
Vector2 mousePos = GetMousePosition();
Vector2 texCoords = RM_UnmapPoint(surface, mousePos);

if (texCoords.x >= 0 && texCoords.x <= 1) {
    // Click is inside surface
    // texCoords is in [0,1] range
}

// Texture → Screen
Vector2 texPoint = { 0.5f, 0.5f };  // Center of texture
Vector2 screenPos = RM_MapPoint(surface, texPoint);
```

---

##  Troubleshooting

### "Failed to create surface!"
- Check that raylib is properly installed
- Verify GPU supports OpenGL 3.3+
- Try smaller surface dimensions

### Examples won't compile
- Make sure `raymap.h` is in parent directory (`../../raymap.h`)
- Check raylib installation: `pkg-config --cflags --libs raylib`
- Verify compiler (gcc/clang) is installed

### Black screen / Nothing visible
- Surface might be warped outside screen bounds
- Press `R` to reset quad
- Check if quad corners are valid (not degenerate)

### Performance issues
- Lower mesh resolution (`DOWN` key in example 04)
- Use BILINEAR instead of HOMOGRAPHY for simple cases
- Reduce surface dimensions if not needed

---

##  Next Steps

After mastering these core examples, check out:

- **examples/video/** - Video playback and mapping
- **examples/calibration/** - Advanced calibration techniques
- **examples/advanced/** - Particle systems, shaders, animations

---

##  Notes

- All examples use **1920x1080** window by default (change in code if needed)
- Config files are saved in the working directory
- Examples are self-contained (no external assets needed)
- Press `ESC` to exit any example

---

