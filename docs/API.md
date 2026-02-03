#  RayMap API Reference v1.0.0

**Complete technical documentation for all RayMap functions**

---

## 📑 Quick Navigation

- [Types & Structures](#types--structures)
- [Surface Management (6 functions)](#surface-management)
- [Rendering (5 functions)](#rendering)
- [Calibration (11 functions)](#calibration)
- [Configuration (2 functions)](#configuration-io)
- [Geometry (4 functions)](#geometry-utilities)
- [Point Mapping (2 functions)](#point-mapping)
- [Examples](#code-examples)

**Total: 30 public API functions**

---

## Types & Structures

### `RM_Quad`

```c
typedef struct {
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomRight;
    Vector2 bottomLeft;
} RM_Quad;
```

4-point quadrilateral defining surface corners in screen space.

**Constraints:**
- Minimum area: 100 pixels²
- Minimum corner distance: 1 pixel
- Non-self-intersecting

---

### `RM_MapMode`

```c
typedef enum {
    RM_MAP_BILINEAR = 0,
    RM_MAP_HOMOGRAPHY
} RM_MapMode;
```

| Mode         | Speed  | Quality   | Use Case      | Default Mesh |
|--------------|--------|-----------|---------------|--------------|
| `BILINEAR`   | Fast   | Good      | Flat surfaces | 16×16        |
| `HOMOGRAPHY` | Medium | Excellent | Angled/tilted | 32×32        |

---

### `RM_Surface`

```c
typedef struct RM_Surface RM_Surface;
```

Opaque pointer. Access only through API functions.

---

### `RM_CalibrationConfig`

```c
typedef struct {
    bool showCorners;
    bool showGrid;
    bool showBorder;
    Color cornerColor;
    Color selectedCornerColor;
    Color gridColor;
    Color borderColor;
    float cornerRadius;
    int gridResolutionX;
    int gridResolutionY;
} RM_CalibrationConfig;
```

---

### `RM_Calibration`

```c
typedef struct {
    RM_Surface *surface;
    RM_CalibrationConfig config;
    int activeCorner;           // -1 or 0-3
    Vector2 dragOffset;
    bool enabled;
} RM_Calibration;
```

---

## Surface Management

### RM_CreateSurface

```c
RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode);
```

**Create new mappable surface**

**Parameters:**
- `width`: 1-8192 pixels
- `height`: 1-8192 pixels  
- `mode`: `RM_MAP_BILINEAR` or `RM_MAP_HOMOGRAPHY`

**Returns:** Surface pointer or `NULL` on failure

**Example:**
```c
RM_Surface *s = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
if (!s) {
    TraceLog(LOG_ERROR, "Failed");
    return;
}
```

---

### RM_DestroySurface

```c
void RM_DestroySurface(RM_Surface *surface);
```

**Destroy surface and free resources**

**Parameters:**
- `surface`: Surface to destroy (can be `NULL`)

**Example:**
```c
RM_DestroySurface(surface);
surface = NULL;
```

---

### RM_SetQuad

```c
bool RM_SetQuad(RM_Surface *surface, RM_Quad quad);
```

**Set corner positions**

**Returns:** `true` if valid, `false` if rejected

**Rejection reasons:**
- Area < 100px²
- Corners too close (< 1px)
- Surface is `NULL`

**Example:**
```c
RM_Quad q = {{100,100}, {700,100}, {700,500}, {100,500}};
if (!RM_SetQuad(surface, q)) {
    TraceLog(LOG_WARNING, "Invalid quad");
}
```

---

### RM_GetQuad

```c
RM_Quad RM_GetQuad(const RM_Surface *surface);
```

**Get current corner positions**

**Returns:** Current quad

---

### RM_GetSurfaceSize

```c
void RM_GetSurfaceSize(const RM_Surface *surface, int *width, int *height);
```

**Get texture dimensions**

**Example:**
```c
int w, h;
RM_GetSurfaceSize(surface, &w, &h);
```

---

### RM_SetMeshResolution

```c
void RM_SetMeshResolution(RM_Surface *surface, int columns, int rows);
```

**Set mesh subdivision (4-64, clamped)**

**Recommendations:**

| Resolution | Vertices | Use Case                          |
|------------|----------|-----------------------------------|
| 8×8        | 81       | Low quality, fast                 |
| 16×16      | 289      | Good (default bilinear)           |
| 32×32      | 1089     | High quality (default homography) |
| 64×64      | 4225     | Maximum quality                   |

**Example:**
```c
RM_SetMeshResolution(surface, 32, 32);
```

---

### RM_GetMeshResolution

```c
void RM_GetMeshResolution(const RM_Surface *surface, int *columns, int *rows);
```

**Get current mesh resolution**

---

## Rendering

### RM_BeginSurface

```c
void RM_BeginSurface(RM_Surface *surface);
```

**Begin drawing to surface**

Must pair with `RM_EndSurface()`. Cannot nest.

**Example:**
```c
RM_BeginSurface(surface);
    ClearBackground(WHITE);
    DrawText("Hello", 100, 100, 40, BLACK);
RM_EndSurface(surface);
```

---

### RM_EndSurface

```c
void RM_EndSurface(RM_Surface *surface);
```

**End drawing to surface**

---

### RM_DrawSurface

```c
void RM_DrawSurface(RM_Surface *surface);
```

**Draw warped surface to screen**

Triggers lazy mesh regeneration if needed.

**Example:**
```c
BeginDrawing();
    ClearBackground(BLACK);
    RM_DrawSurface(surface);
EndDrawing();
```

---

### RM_SetMapMode

```c
void RM_SetMapMode(RM_Surface *surface, RM_MapMode mode);
```

**Change mapping algorithm**

Updates mesh resolution to mode default.

---

### RM_GetMapMode

```c
RM_MapMode RM_GetMapMode(const RM_Surface *surface);
```

**Get current mapping mode**

---

## Calibration

### RM_CalibrationDefault

```c
RM_Calibration RM_CalibrationDefault(RM_Surface *surface);
```

**Create calibration with defaults**

**Example:**
```c
RM_Calibration calib = RM_CalibrationDefault(surface);
```

---

### RM_UpdateCalibration

```c
void RM_UpdateCalibration(RM_Calibration *calibration);
```

**Update input and corner dragging**

Call every frame when active.

---

### RM_UpdateCalibrationInput

```c
void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey);
```

**Update with auto-toggle (0 to disable)**

**Example:**
```c
RM_UpdateCalibrationInput(&calib, KEY_TAB);  // TAB toggles
```

---

### RM_ToggleCalibration

```c
void RM_ToggleCalibration(RM_Calibration *calibration);
```

**Manually toggle on/off**

---

### RM_DrawCalibration

```c
void RM_DrawCalibration(RM_Calibration calibration);
```

**Draw full overlay (corners + grid + border)**

---

### RM_DrawCalibrationCorners

```c
void RM_DrawCalibrationCorners(RM_Calibration calibration);
```

**Draw only corner handles**

---

### RM_DrawCalibrationBorder

```c
void RM_DrawCalibrationBorder(RM_Calibration calibration);
```

**Draw only quad border**

---

### RM_DrawCalibrationGrid

```c
void RM_DrawCalibrationGrid(RM_Calibration calibration);
```

**Draw only deformation grid**

---

### RM_ResetCalibrationQuad

```c
void RM_ResetCalibrationQuad(RM_Calibration *calibration, 
                             int screenWidth, int screenHeight);
```

**Reset to centered rectangle**

**Example:**
```c
if (IsKeyPressed(KEY_R)) {
    RM_ResetCalibrationQuad(&calib, 1920, 1080);
}
```

---

### RM_ResetQuad

```c
void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight);
```

**Reset quad directly (no calibration needed)**

---

### RM_GetActiveCorner

```c
int RM_GetActiveCorner(RM_Calibration calibration);
```

**Get grabbed corner index**

**Returns:** 0-3 or -1

**Corner indices:**
```
0 (topLeft)      1 (topRight)
    ┌────────────┐
    │            │
    └────────────┘
3 (bottomLeft)   2 (bottomRight)
```

---

### RM_IsCalibrating

```c
bool RM_IsCalibrating(RM_Calibration calibration);
```

**Check if actively dragging**

---

## Configuration I/O

### RM_SaveConfig

```c
bool RM_SaveConfig(const RM_Surface *surface, const char *filepath);
```

**Save to text file**

**Format:**
```ini
[Surface]
width=800
height=600

[Mode]
mode=BILINEAR

[Mesh]
columns=16
rows=16

[Quad]
topLeft=100.00,100.00
topRight=700.00,100.00
bottomRight=700.00,500.00
bottomLeft=100.00,500.00
```

**Example:**
```c
RM_SaveConfig(surface, "calib.txt");
```

---

### RM_LoadConfig

```c
bool RM_LoadConfig(RM_Surface *surface, const char *filepath);
```

**Load from text file**

**Example:**
```c
if (!RM_LoadConfig(surface, "calib.txt")) {
    TraceLog(LOG_WARNING, "Using defaults");
}
```

---

## Geometry Utilities

### RM_PointInQuad

```c
bool RM_PointInQuad(Vector2 point, RM_Quad quad);
```

**Test if point inside quad**

**Example:**
```c
if (RM_PointInQuad(GetMousePosition(), quad)) {
    DrawText("Inside!", 10, 10, 20, GREEN);
}
```

---

### RM_GetQuadBounds

```c
Rectangle RM_GetQuadBounds(RM_Quad quad);
```

**Get axis-aligned bounding box**

---

### RM_GetQuadCenter

```c
Vector2 RM_GetQuadCenter(RM_Quad quad);
```

**Get center point (average of corners)**

---

### RM_GetQuadArea

```c
float RM_GetQuadArea(RM_Quad quad);
```

**Calculate area in pixels²**

---

## Point Mapping

### RM_MapPoint

```c
Vector2 RM_MapPoint(RM_Surface *surface, Vector2 texturePoint);
```

**Map texture [0,1] to screen space**

**Example:**
```c
Vector2 center = {0.5f, 0.5f};
Vector2 screen = RM_MapPoint(surface, center);
DrawCircle(screen.x, screen.y, 5, GREEN);
```

---

### RM_UnmapPoint

```c
Vector2 RM_UnmapPoint(RM_Surface *surface, Vector2 screenPoint);
```

**Map screen to texture [0,1]**

**Returns:** UV coords or (-1,-1) if outside

**Example:**
```c
Vector2 uv = RM_UnmapPoint(surface, GetMousePosition());
if (uv.x >= 0) {
    printf("UV: (%.2f, %.2f)\n", uv.x, uv.y);
}
```

---

## Code Examples

### Minimal Example

```c
#include "raylib.h"
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

int main(void) {
    InitWindow(1280, 720, "RayMap");
    RM_Surface *s = RM_CreateSurface(640, 480, RM_MAP_BILINEAR);
    
    while (!WindowShouldClose()) {
        RM_BeginSurface(s);
            ClearBackground(WHITE);
            DrawText("Hello!", 200, 200, 40, BLACK);
        RM_EndSurface(s);
        
        BeginDrawing();
            ClearBackground(BLACK);
            RM_DrawSurface(s);
        EndDrawing();
    }
    
    RM_DestroySurface(s);
    CloseWindow();
    return 0;
}
```

---

### Interactive Calibration

```c
RM_Surface *s = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
RM_Calibration calib = RM_CalibrationDefault(s);

while (!WindowShouldClose()) {
    RM_UpdateCalibrationInput(&calib, KEY_TAB);
    
    if (IsKeyPressed(KEY_S)) {
        RM_SaveConfig(s, "calib.txt");
    }
    if (IsKeyPressed(KEY_L)) {
        RM_LoadConfig(s, "calib.txt");
    }
    
    RM_BeginSurface(s);
        ClearBackground(BLUE);
    RM_EndSurface(s);
    
    BeginDrawing();
        ClearBackground(BLACK);
        RM_DrawSurface(s);
        RM_DrawCalibration(calib);
    EndDrawing();
}
```

---

### Multiple Surfaces

```c
RM_Surface *surfaces[3];
for (int i = 0; i < 3; i++) {
    surfaces[i] = RM_CreateSurface(640, 480, RM_MAP_BILINEAR);
}

// Position each surface
RM_Quad q1 = {{100, 100}, {740, 100}, {740, 580}, {100, 580}};
RM_Quad q2 = {{800, 100}, {1440, 100}, {1440, 580}, {800, 580}};
RM_Quad q3 = {{450, 650}, {1050, 650}, {1050, 1050}, {450, 1050}};
RM_SetQuad(surfaces[0], q1);
RM_SetQuad(surfaces[1], q2);
RM_SetQuad(surfaces[2], q3);

// Draw all
BeginDrawing();
    for (int i = 0; i < 3; i++) {
        RM_DrawSurface(surfaces[i]);
    }
EndDrawing();
```

---

### Mesh Quality Adjustment

```c
// For performance
RM_SetMeshResolution(surface, 8, 8);

// For quality
RM_SetMeshResolution(surface, 64, 64);

// Query
int cols, rows;
RM_GetMeshResolution(surface, &cols, &rows);
printf("Mesh: %dx%d (%d vertices)\n", 
       cols, rows, (cols+1)*(rows+1));
```

---

## Performance Guide

### Mesh Resolution Impact

| Resolution | Vertices | Triangles | FPS @ 1080p |
|------------|----------|-----------|-------------|
| 8×8        | 81       | 128       | 200+        |
| 16×16      | 289      | 512       | 144+        |
| 32×32      | 1089     | 2048      | 90+         |
| 64×64      | 4225     | 8192      | 60+         |

### Update Optimization

```c
// GOOD: Update only on change
if (contentChanged) {
    RM_BeginSurface(surface);
        DrawContent();
    RM_EndSurface(surface);
}
RM_DrawSurface(surface);  // Always cheap

// BAD: Update every frame
RM_BeginSurface(surface);  // Expensive!
    DrawContent();
RM_EndSurface(surface);
```

### Multiple Surfaces

**Recommended limits:**
- 3-5 surfaces: No issues
- 5-10 surfaces: Reduce mesh resolution
- 10+ surfaces: Profile carefully

---

## Troubleshooting

| Issue                   | Solution                                |
|-------------------------|-----------------------------------------|
| Creation fails          | Check dimensions 1-8192, GPU memory     |
| Distorted mesh          | Increase resolution, check quad order   |
| Performance drop        | Reduce mesh resolution or surface count |
| Calibration not working | Verify `enabled=true`, call Update()    |
| Config load fails       | Check file exists, format valid         |

---

## Memory & Safety

✅ **Leak-free:** Verified with Valgrind + AddressSanitizer  
✅ **Safe NULL handling:** All functions check pointers  
✅ **No global state:** Thread-local safe (if Raylib is)  
✅ **RAII-style:** Resources freed on destroy

---

## Platform Support

| Platform | Status  | Compiler          | Link Flags     |
|----------|---------|-------------------|----------------|
| Linux    |  Tested | GCC 9+, Clang 10+ | `-lraylib -lm` |

---

## Version Info

**API Version:** 1.0.0  
**Stable API:** Yes (no breaking changes planned)  
**ABI Stable:** No (opaque pointer, single-header)

---

## License

zlib - Free for commercial use

Copyright (c) 2025 grerfou

---

**Documentation last updated:** 2025-01-XX  
**For latest version:** [GitHub](https://github.com/grerfou/raymap)
