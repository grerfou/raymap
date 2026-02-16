# RayMap API Reference

**Version:** 1.1.0  
**Last Updated:** February 2026

Complete API documentation for RayMap projection mapping library.

---

## Table of Contents

- [Core Types](#core-types)
- [Surface Management](#surface-management)
- [Rendering](#rendering)
- [Calibration](#calibration)
- [Configuration I/O](#configuration-io)
- [Geometry Utilities](#geometry-utilities)
- [Point Mapping](#point-mapping)
- [Video Extension (RayMapVid)](#video-extension-raymapvid)
- [Constants & Macros](#constants--macros)
- [Error Handling](#error-handling)

---

## Core Types

### RM_Quad

```c
typedef struct {
    Vector2 topLeft;
    Vector2 topRight;
    Vector2 bottomRight;
    Vector2 bottomLeft;
} RM_Quad;
```

**Description:**  
Defines a 4-point quadrilateral in screen space. Corner order is counter-clockwise starting from top-left.

**Fields:**
- `topLeft` - Top-left corner position (screen coordinates)
- `topRight` - Top-right corner position
- `bottomRight` - Bottom-right corner position
- `bottomLeft` - Bottom-left corner position

**Example:**
```c
RM_Quad quad = {
    .topLeft = {100, 100},
    .topRight = {1820, 100},
    .bottomRight = {1820, 980},
    .bottomLeft = {100, 980}
};
```

**Notes:**
- Coordinates are in pixels (screen space)
- Quad should be convex for best results
- Minimum area: 100 square pixels
- Minimum corner distance: 1 pixel

---

### RM_MapMode

```c
typedef enum {
    RM_MAP_BILINEAR = 0,    // Simple bilinear interpolation
    RM_MAP_HOMOGRAPHY       // Perspective-correct homography
} RM_MapMode;
```

**Description:**  
Mapping algorithm used for surface warping.

**Values:**

| Mode | Description | Use Case | Default Mesh |
|------|-------------|----------|--------------|
| `RM_MAP_BILINEAR` | Fast bilinear interpolation | Flat/slightly curved surfaces | 16×16 |
| `RM_MAP_HOMOGRAPHY` | Perspective-correct transform | Projections, 3D surfaces | 32×32 |

**Performance:**
- Bilinear: ~10% faster
- Homography: More accurate for perspective

---

### RM_Surface

```c
typedef struct RM_Surface RM_Surface;  // Opaque type
```

**Description:**  
Opaque handle to a mappable surface. Contains render texture, mesh, and transformation state.

**Lifecycle:**
```c
RM_Surface *surf = RM_CreateSurface(...);  // Create
// ... use surface ...
RM_DestroySurface(surf);                    // Destroy
```

**Internal State (not directly accessible):**
- Render texture (user draws to this)
- Deformed mesh (GPU geometry)
- Quad corner positions
- Homography matrix (cached)
- Mesh resolution settings
- Dirty flags for lazy updates

---

### RM_CalibrationConfig

```c
typedef struct {
    bool showCorners;               // Display corner handles
    bool showGrid;                  // Display deformation grid
    bool showBorder;                // Display quad border
    Color cornerColor;              // Default corner color
    Color selectedCornerColor;      // Active corner color
    Color gridColor;                // Grid line color
    Color borderColor;              // Border color
    float cornerRadius;             // Corner handle radius (pixels)
    int gridResolutionX;            // Horizontal grid subdivisions
    int gridResolutionY;            // Vertical grid subdivisions
} RM_CalibrationConfig;
```

**Description:**  
Visual configuration for calibration overlay.

**Default Values:**
```c
{
    .showCorners = true,
    .showGrid = true,
    .showBorder = true,
    .cornerColor = YELLOW,
    .selectedCornerColor = GREEN,
    .gridColor = ColorAlpha(WHITE, 0.3f),
    .borderColor = RED,
    .cornerRadius = 15.0f,
    .gridResolutionX = 8,
    .gridResolutionY = 8
}
```

**Customization:**
```c
RM_Calibration calib = RM_CalibrationDefault(surface);
calib.config.cornerRadius = 20.0f;        // Bigger handles
calib.config.gridColor = ColorAlpha(BLUE, 0.5f);
calib.config.showGrid = false;            // Hide grid
```

---

### RM_Calibration

```c
typedef struct {
    RM_Surface *surface;            // Target surface
    RM_CalibrationConfig config;    // Visual settings
    int activeCorner;               // Currently selected corner (-1 if none)
    Vector2 dragOffset;             // Mouse drag offset
    bool enabled;                   // Calibration mode active/inactive
} RM_Calibration;
```

**Description:**  
Calibration state for interactive corner adjustment.

**Usage Pattern:**
```c
RM_Calibration calib = RM_CalibrationDefault(surface);

// In update loop:
RM_UpdateCalibrationInput(&calib, KEY_C);  // Toggle with C

// In draw loop:
RM_DrawCalibration(calib);
```

**Corner Indices:**
- `0` - Top-left
- `1` - Top-right
- `2` - Bottom-right
- `3` - Bottom-left
- `-1` - No corner selected

---

## Surface Management

### RM_CreateSurface

```c
RM_Surface *RM_CreateSurface(int width, int height, RM_MapMode mode);
```

**Description:**  
Creates a new mappable surface with render texture and mesh.

**Parameters:**
- `width` - Render texture width in pixels (1-8192)
- `height` - Render texture height in pixels (1-8192)
- `mode` - Mapping algorithm (`RM_MAP_BILINEAR` or `RM_MAP_HOMOGRAPHY`)

**Returns:**
- Pointer to new surface on success
- `NULL` on failure (check logs)

**Errors:**
- Invalid dimensions (≤0 or >8192)
- Memory allocation failure
- GPU texture creation failure
- Mesh generation failure

**Example:**
```c
// HD surface with homography
RM_Surface *surf = RM_CreateSurface(1920, 1080, RM_MAP_HOMOGRAPHY);
if (!surf) {
    TraceLog(LOG_ERROR, "Failed to create surface!");
    return -1;
}
```

**Notes:**
- Initial quad is centered rectangle matching texture dimensions
- Mesh is generated immediately
- Default resolution: 16×16 (bilinear) or 32×32 (homography)

---

### RM_DestroySurface

```c
void RM_DestroySurface(RM_Surface *surface);
```

**Description:**  
Destroys surface and frees all resources (texture, mesh, memory).

**Parameters:**
- `surface` - Surface to destroy (can be `NULL`, safe to call)

**Example:**
```c
RM_DestroySurface(surface);
surface = NULL;  // Good practice
```

**Cleanup Order:**
1. Mesh (GPU buffers + CPU arrays)
2. Material
3. Render texture
4. Surface struct

**Thread Safety:**  
Not thread-safe. Do not destroy while drawing.

---

### RM_SetQuad

```c
bool RM_SetQuad(RM_Surface *surface, RM_Quad quad);
```

**Description:**  
Sets quad corner positions. Validates and applies transformation.

**Parameters:**
- `surface` - Target surface
- `quad` - New quad corner positions

**Returns:**
- `true` if quad is valid and applied
- `false` if quad is rejected (surface unchanged)

**Validation:**
- Minimum area: 100 square pixels
- Minimum corner distance: 1 pixel
- All corners must be distinct

**Example:**
```c
RM_Quad trapezoid = {
    {200, 100},   // Top-left
    {1720, 100},  // Top-right (wider)
    {1600, 980},  // Bottom-right (narrower)
    {320, 980}    // Bottom-left (narrower)
};

if (!RM_SetQuad(surface, trapezoid)) {
    TraceLog(LOG_WARNING, "Invalid quad rejected");
}
```

**Side Effects:**
- Sets `meshNeedsUpdate = true`
- Sets `homographyNeedsUpdate = true`
- Mesh regenerated on next `RM_DrawSurface()`

---

### RM_GetQuad

```c
RM_Quad RM_GetQuad(const RM_Surface *surface);
```

**Description:**  
Gets current quad corner positions.

**Parameters:**
- `surface` - Surface to query

**Returns:**
- Current quad positions
- Zero quad if surface is `NULL`

**Example:**
```c
RM_Quad current = RM_GetQuad(surface);
TraceLog(LOG_INFO, "Top-left: %.0f, %.0f", 
         current.topLeft.x, current.topLeft.y);
```

---

### RM_GetSurfaceSize

```c
void RM_GetSurfaceSize(const RM_Surface *surface, int *width, int *height);
```

**Description:**  
Gets render texture dimensions.

**Parameters:**
- `surface` - Surface to query
- `width` - Output: texture width (can be `NULL`)
- `height` - Output: texture height (can be `NULL`)

**Example:**
```c
int w, h;
RM_GetSurfaceSize(surface, &w, &h);
TraceLog(LOG_INFO, "Surface size: %dx%d", w, h);
```

---

### RM_SetMeshResolution

```c
void RM_SetMeshResolution(RM_Surface *surface, int columns, int rows);
```

**Description:**  
Sets mesh subdivision resolution. Higher = smoother warping, lower performance.

**Parameters:**
- `surface` - Target surface
- `columns` - Horizontal subdivisions (4-64, clamped)
- `rows` - Vertical subdivisions (4-64, clamped)

**Example:**
```c
// High detail for curved surface
RM_SetMeshResolution(surface, 64, 64);  // 4225 vertices

// Low detail for flat surface
RM_SetMeshResolution(surface, 8, 8);    // 81 vertices
```

**Performance Impact:**

| Resolution | Vertices | Triangles | FPS (RTX 3060) |
|------------|----------|-----------|----------------|
| 4×4        | 25       | 32        | 3000+          |
| 16×16      | 289      | 512       | 2000+          |
| 32×32      | 1089     | 2048      | 1500+          |
| 64×64      | 4225     | 8192      | 800+           |

**Notes:**
- No effect if values unchanged
- Mesh regenerated on next draw
- Higher resolution needed for homography mode

---

### RM_GetMeshResolution

```c
void RM_GetMeshResolution(const RM_Surface *surface, int *columns, int *rows);
```

**Description:**  
Gets current mesh resolution.

**Parameters:**
- `surface` - Surface to query
- `columns` - Output: horizontal subdivisions (can be `NULL`)
- `rows` - Output: vertical subdivisions (can be `NULL`)

**Example:**
```c
int cols, rows;
RM_GetMeshResolution(surface, &cols, &rows);
TraceLog(LOG_INFO, "Mesh: %dx%d", cols, rows);
```

---

## Rendering

### RM_BeginSurface

```c
void RM_BeginSurface(RM_Surface *surface);
```

**Description:**  
Begins drawing to surface render texture. All raylib draw calls will be captured.

**Parameters:**
- `surface` - Surface to draw to

**Example:**
```c
RM_BeginSurface(surface);
    ClearBackground(BLACK);
    DrawText("Content", 100, 100, 40, WHITE);
    DrawCircle(400, 300, 50, RED);
RM_EndSurface(surface);
```

**Notes:**
- Must be paired with `RM_EndSurface()`
- Cannot nest surface drawing
- Equivalent to `BeginTextureMode(surface->target)`

---

### RM_EndSurface

```c
void RM_EndSurface(RM_Surface *surface);
```

**Description:**  
Ends drawing to surface render texture.

**Parameters:**
- `surface` - Surface being drawn to

**Notes:**
- Must match previous `RM_BeginSurface()`
- Equivalent to `EndTextureMode()`

---

### RM_DrawSurface

```c
void RM_DrawSurface(RM_Surface *surface);
```

**Description:**  
Draws the warped surface to the screen. Applies mesh transformation.

**Parameters:**
- `surface` - Surface to render

**Example:**
```c
BeginDrawing();
    ClearBackground(BLACK);
    RM_DrawSurface(surface);  // Draws warped surface
EndDrawing();
```

**Internal Behavior:**
1. Lazy mesh update (if quad changed)
2. Validate mesh/texture
3. Disable depth test/backface culling
4. Draw mesh with texture
5. Restore GL state

**Performance:**
- Mesh update only when needed (dirty flag)
- Single draw call per surface
- GPU-accelerated transformation

---

### RM_SetMapMode

```c
void RM_SetMapMode(RM_Surface *surface, RM_MapMode mode);
```

**Description:**  
Changes mapping algorithm. Triggers mesh regeneration.

**Parameters:**
- `surface` - Target surface
- `mode` - New mapping mode

**Example:**
```c
// Switch to homography for better perspective
RM_SetMapMode(surface, RM_MAP_HOMOGRAPHY);

// Toggle mode
RM_MapMode current = RM_GetMapMode(surface);
RM_SetMapMode(surface, 
    current == RM_MAP_BILINEAR ? RM_MAP_HOMOGRAPHY : RM_MAP_BILINEAR);
```

**Side Effects:**
- Updates default mesh resolution for new mode
- Sets `meshNeedsUpdate = true`
- No-op if mode unchanged

---

### RM_GetMapMode

```c
RM_MapMode RM_GetMapMode(const RM_Surface *surface);
```

**Description:**  
Gets current mapping mode.

**Parameters:**
- `surface` - Surface to query

**Returns:**
- Current map mode
- `RM_MAP_BILINEAR` if surface is `NULL`

---

## Calibration

### RM_CalibrationDefault

```c
RM_Calibration RM_CalibrationDefault(RM_Surface *surface);
```

**Description:**  
Creates calibration state with default visual settings.

**Parameters:**
- `surface` - Surface to calibrate

**Returns:**
- Calibration struct with default config
- Enabled by default

**Example:**
```c
RM_Calibration calib = RM_CalibrationDefault(surface);

// Customize
calib.config.cornerRadius = 20.0f;
calib.config.borderColor = BLUE;
```

**Default Config:**
- Yellow corners (green when selected)
- White semi-transparent grid (8×8)
- Red border
- 15px corner radius
- All overlays enabled

---

### RM_UpdateCalibration

```c
void RM_UpdateCalibration(RM_Calibration *calibration);
```

**Description:**  
Updates calibration state. Handles mouse input for corner dragging.

**Parameters:**
- `calibration` - Calibration to update

**Example:**
```c
// In main loop (update section)
if (calibration.enabled) {
    RM_UpdateCalibration(&calibration);
}
```

**Input Handling:**
1. **Mouse Down** - Check if clicked on corner (within radius × 1.5)
2. **Mouse Drag** - Move active corner with offset
3. **Mouse Up** - Release corner

**Notes:**
- Only processes input if `enabled == true`
- Call every frame when calibration is active
- Does not toggle enabled state (use `RM_ToggleCalibration` or `RM_UpdateCalibrationInput`)

---

### RM_UpdateCalibrationInput

```c
void RM_UpdateCalibrationInput(RM_Calibration *calibration, int toggleKey);
```

**Description:**  
Updates calibration with automatic toggle key handling.

**Parameters:**
- `calibration` - Calibration to update
- `toggleKey` - Key to toggle calibration (0 = no toggle, e.g., `KEY_C`)

**Example:**
```c
// Toggle with C, drag corners with mouse
RM_UpdateCalibrationInput(&calib, KEY_C);
```

**Equivalent To:**
```c
if (toggleKey != 0 && IsKeyPressed(toggleKey)) {
    RM_ToggleCalibration(calibration);
}
RM_UpdateCalibration(calibration);
```

---

### RM_ToggleCalibration

```c
void RM_ToggleCalibration(RM_Calibration *calibration);
```

**Description:**  
Toggles calibration on/off. Releases active corner when disabled.

**Parameters:**
- `calibration` - Calibration to toggle

**Example:**
```c
if (IsKeyPressed(KEY_C)) {
    RM_ToggleCalibration(&calib);
}
```

**State Changes:**
- `enabled = !enabled`
- `activeCorner = -1` (if disabling)

---

### RM_DrawCalibration

```c
void RM_DrawCalibration(RM_Calibration calibration);
```

**Description:**  
Draws complete calibration overlay (border + grid + corners).

**Parameters:**
- `calibration` - Calibration to draw

**Example:**
```c
BeginDrawing();
    RM_DrawSurface(surface);
    RM_DrawCalibration(calib);  // Draw overlay on top
EndDrawing();
```

**Draw Order:**
1. Border (if `showBorder`)
2. Grid (if `showGrid`)
3. Corners (if `showCorners`)

**Equivalent To:**
```c
RM_DrawCalibrationBorder(calib);
RM_DrawCalibrationGrid(calib);
RM_DrawCalibrationCorners(calib);
```

---

### RM_DrawCalibrationCorners

```c
void RM_DrawCalibrationCorners(RM_Calibration calibration);
```

**Description:**  
Draws only corner handles with labels.

**Parameters:**
- `calibration` - Calibration to draw

**Visual:**
- Filled circle (color depends on selection state)
- White outline
- Black text label (0-3)

---

### RM_DrawCalibrationBorder

```c
void RM_DrawCalibrationBorder(RM_Calibration calibration);
```

**Description:**  
Draws only quad border lines.

**Parameters:**
- `calibration` - Calibration to draw

**Visual:**
- 2px thick lines connecting corners
- Color: `config.borderColor`

---

### RM_DrawCalibrationGrid

```c
void RM_DrawCalibrationGrid(RM_Calibration calibration);
```

**Description:**  
Draws only deformation grid.

**Parameters:**
- `calibration` - Calibration to draw

**Visual:**
- Horizontal lines (top to bottom interpolation)
- Vertical lines (left to right interpolation)
- Resolution from `config.gridResolutionX/Y`
- Color: `config.gridColor`

---

### RM_ResetCalibrationQuad

```c
void RM_ResetCalibrationQuad(RM_Calibration *calibration, 
                              int screenWidth, int screenHeight);
```

**Description:**  
Resets quad to centered rectangle matching surface dimensions.

**Parameters:**
- `calibration` - Calibration to reset
- `screenWidth` - Screen width for centering
- `screenHeight` - Screen height for centering

**Example:**
```c
if (IsKeyPressed(KEY_R)) {
    RM_ResetCalibrationQuad(&calib, 1920, 1080);
}
```

**Equivalent To:**
```c
RM_ResetQuad(calibration->surface, screenWidth, screenHeight);
```

---

### RM_ResetQuad

```c
void RM_ResetQuad(RM_Surface *surface, int screenWidth, int screenHeight);
```

**Description:**  
Resets surface quad to centered rectangle.

**Parameters:**
- `surface` - Surface to reset
- `screenWidth` - Screen width
- `screenHeight` - Screen height

**Example:**
```c
// Reset to center of 1920x1080 screen
RM_ResetQuad(surface, 1920, 1080);
```

**Calculation:**
```c
int x = (screenWidth - surface->width) / 2;
int y = (screenHeight - surface->height) / 2;

// Quad becomes:
// (x, y) ----------- (x+w, y)
//   |                    |
// (x, y+h) --------- (x+w, y+h)
```

---

### RM_GetActiveCorner

```c
int RM_GetActiveCorner(RM_Calibration calibration);
```

**Description:**  
Gets index of currently selected corner.

**Parameters:**
- `calibration` - Calibration to query

**Returns:**
- `0-3` - Corner index
- `-1` - No corner selected

---

### RM_IsCalibrating

```c
bool RM_IsCalibrating(RM_Calibration calibration);
```

**Description:**  
Checks if user is currently dragging a corner.

**Parameters:**
- `calibration` - Calibration to check

**Returns:**
- `true` if actively dragging corner
- `false` otherwise

**Logic:**
```c
return (calibration.activeCorner >= 0 && IsMouseButtonDown(MOUSE_LEFT_BUTTON));
```

---

## Configuration I/O

### RM_SaveConfig

```c
bool RM_SaveConfig(const RM_Surface *surface, const char *filepath);
```

**Description:**  
Saves surface configuration to text file.

**Parameters:**
- `surface` - Surface to save
- `filepath` - Output file path

**Returns:**
- `true` on success
- `false` on failure

**File Format:**
```ini
# RAYMAP Config File
# Format: text/plain v1.0

[Surface]
width=1920
height=1080

[Mode]
mode=HOMOGRAPHY

[Mesh]
columns=32
rows=32

[Quad]
topLeft=200.00,100.00
topRight=1720.00,100.00
bottomRight=1600.00,980.00
bottomLeft=320.00,980.00
```

**Example:**
```c
if (IsKeyPressed(KEY_S)) {
    if (RM_SaveConfig(surface, "calibration.cfg")) {
        TraceLog(LOG_INFO, "Saved!");
    }
}
```

**Errors:**
- `NULL` parameters
- File cannot be opened for writing
- Disk full

---

### RM_LoadConfig

```c
bool RM_LoadConfig(RM_Surface *surface, const char *filepath);
```

**Description:**  
Loads surface configuration from text file.

**Parameters:**
- `surface` - Surface to configure
- `filepath` - Input file path

**Returns:**
- `true` on success
- `false` on failure

**Example:**
```c
if (IsKeyPressed(KEY_L)) {
    if (RM_LoadConfig(surface, "calibration.cfg")) {
        TraceLog(LOG_INFO, "Loaded!");
    }
}
```

**Parsing:**
- Ignores comments (`#`)
- Ignores empty lines
- Ignores section headers (`[...]`)
- Parses `key=value` pairs

**Applied Settings:**
1. Mapping mode
2. Mesh resolution
3. Quad corners

**Errors:**
- File not found
- Invalid format
- No quad data found

**Notes:**
- Surface dimensions not validated (must match)
- Quad validated (min area, distinct corners)

---

## Geometry Utilities

### RM_PointInQuad

```c
bool RM_PointInQuad(Vector2 point, RM_Quad quad);
```

**Description:**  
Tests if point is inside quad using same-side method.

**Parameters:**
- `point` - Point to test (screen coordinates)
- `quad` - Quad to test against

**Returns:**
- `true` if point inside quad
- `false` if outside

**Example:**
```c
Vector2 mousePos = GetMousePosition();
if (RM_PointInQuad(mousePos, quad)) {
    DrawText("Inside!", 10, 10, 20, GREEN);
}
```

**Algorithm:**
1. Compute quad center
2. Check if point is on same side of each edge as center
3. All 4 checks must pass

**Limitations:**
- Assumes convex quad
- May fail for highly concave quads

---

### RM_GetQuadBounds

```c
Rectangle RM_GetQuadBounds(RM_Quad quad);
```

**Description:**  
Computes axis-aligned bounding box of quad.

**Parameters:**
- `quad` - Quad to bound

**Returns:**
- Rectangle containing quad (min/max of all corners)

**Example:**
```c
Rectangle bounds = RM_GetQuadBounds(quad);
DrawRectangleLinesEx(bounds, 2, BLUE);  // Debug visualization
```

**Uses:**
- Culling tests
- UI layout
- Click detection optimization

---

### RM_GetQuadCenter

```c
Vector2 RM_GetQuadCenter(RM_Quad quad);
```

**Description:**  
Computes geometric center (average of corners).

**Parameters:**
- `quad` - Quad to center

**Returns:**
- Center point

**Formula:**
```c
center.x = (topLeft.x + topRight.x + bottomRight.x + bottomLeft.x) / 4
center.y = (topLeft.y + topRight.y + bottomRight.y + bottomLeft.y) / 4
```

**Example:**
```c
Vector2 center = RM_GetQuadCenter(quad);
DrawCircle(center.x, center.y, 10, YELLOW);  // Mark center
```

---

### RM_GetQuadArea

```c
float RM_GetQuadArea(RM_Quad quad);
```

**Description:**  
Calculates quad area using shoelace formula.

**Parameters:**
- `quad` - Quad to measure

**Returns:**
- Area in square pixels

**Formula:**
```c
area = 0.5 * |x1(y2-y4) + x2(y3-y1) + x3(y4-y2) + x4(y1-y3)|
```

**Example:**
```c
float area = RM_GetQuadArea(quad);
TraceLog(LOG_INFO, "Quad area: %.0f px²", area);
```

**Uses:**
- Validation (min area = 100px²)
- Quality metrics
- Deformation analysis

---

## Point Mapping

### RM_MapPoint

```c
Vector2 RM_MapPoint(RM_Surface *surface, Vector2 texturePoint);
```

**Description:**  
Maps point from texture space [0,1] to screen space using current transformation.

**Parameters:**
- `surface` - Surface with transformation
- `texturePoint` - Point in texture UV coordinates (0,0 = top-left, 1,1 = bottom-right)

**Returns:**
- Point in screen coordinates
- `(-1, -1)` if surface is `NULL`

**Example:**
```c
// Map texture center (0.5, 0.5) to screen
Vector2 texCenter = {0.5f, 0.5f};
Vector2 screenPos = RM_MapPoint(surface, texCenter);

DrawCircle(screenPos.x, screenPos.y, 10, RED);
```

**Algorithm:**
- **Bilinear mode**: Bilinear interpolation between quad corners
- **Homography mode**: Apply homography matrix transformation

**Clamping:**
- Input clamped to [0,1]
- Output can be anywhere in screen space

---

### RM_UnmapPoint

```c
Vector2 RM_UnmapPoint(RM_Surface *surface, Vector2 screenPoint);
```

**Description:**  
Maps point from screen space to texture space [0,1] using inverse transformation.

**Parameters:**
- `surface` - Surface with transformation
- `screenPoint` - Point in screen coordinates

**Returns:**
- Point in texture UV coordinates [0,1]
- `(-1, -1)` if point is outside quad or surface is `NULL`

**Example:**
```c
if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    Vector2 texCoords = RM_UnmapPoint(surface, mousePos);
    
    if (texCoords.x >= 0) {
        TraceLog(LOG_INFO, "Clicked at UV: %.2f, %.2f", 
                 texCoords.x, texCoords.y);
    }
}
```

**Algorithm:**
1. Check if point inside quad (`RM_PointInQuad`)
2. Compute inverse homography matrix
3. Apply inverse transform
4. Clamp result to [0,1]

**Notes:**
- **Always uses homography inverse** (even in bilinear mode)
- Returns `(-1,-1)` if point outside quad
- Clamped output ensures valid texture coordinates

**Use Cases:**
- Click detection on warped surface
- Touch input mapping
- Interactive content

---

## Video Extension (RayMapVid)

### RMV_Video

```c
typedef struct RMV_Video RMV_Video;  // Opaque type
```

**Description:**  
Opaque handle to video decoder and playback state.

**Lifecycle:**
```c
RMV_Video *video = RMV_LoadVideo("video.mp4");
// ... use video ...
RMV_UnloadVideo(video);
```

---

### RMV_VideoInfo

```c
typedef struct {
    int width;                      // Video width in pixels
    int height;                     // Video height in pixels
    float duration;                 // Duration in seconds
    float fps;                      // Frames per second
    const char *codec;              // Codec name (e.g., "h264")
    const char *format;             // Container format (e.g., "mp4")
    bool hasAudio;                  // Audio stream present
    RMV_HWAccelType hwaccel;        // Active hardware acceleration
} RMV_VideoInfo;
```

**Description:**  
Video metadata and properties.

**Example:**
```c
RMV_VideoInfo info = RMV_GetVideoInfo(video);
TraceLog(LOG_INFO, "Video: %dx%d @ %.0f fps, codec=%s", 
         info.width, info.height, info.fps, info.codec);
```

---

### RMV_PlaybackState

```c
typedef enum {
    RMV_STATE_STOPPED = 0,
    RMV_STATE_PLAYING,
    RMV_STATE_PAUSED,
    RMV_STATE_ERROR
} RMV_PlaybackState;
```

**Description:**  
Video playback state.

---

### RMV_LoadVideo

```c
RMV_Video *RMV_LoadVideo(const char *filepath);
```

**Description:**  
Loads video file and initializes decoder.

**Parameters:**
- `filepath` - Path to video file

**Returns:**
- Video handle on success
- `NULL` on failure

**Supported Formats:**
- MP4, MOV, AVI, MKV, WebM
- Codecs: H.264, H.265/HEVC, VP8, VP9, AV1

**Example:**
```c
RMV_Video *video = RMV_LoadVideo("content.mp4");
if (!video) {
    TraceLog(LOG_ERROR, "Failed to load video!");
    return -1;
}
```

**Errors:**
- File not found
- Unsupported codec
- Invalid video stream
- Memory allocation failure

---

### RMV_UnloadVideo

```c
void RMV_UnloadVideo(RMV_Video *video);
```

**Description:**  
Unloads video and frees all resources.

**Parameters:**
- `video` - Video to unload (can be `NULL`)

**Example:**
```c
RMV_UnloadVideo(video);
video = NULL;
```

---

### RMV_GetVideoInfo

```c
RMV_VideoInfo RMV_GetVideoInfo(const RMV_Video *video);
```

**Description:**  
Gets video metadata.

**Parameters:**
- `video` - Video to query

**Returns:**
- Video info struct
- Zero struct if video is `NULL` or unloaded

---

### RMV_GetVideoTexture

```c
Texture2D RMV_GetVideoTexture(const RMV_Video *video);
```

**Description:**  
Gets current frame as raylib texture.

**Parameters:**
- `video` - Video to get texture from

**Returns:**
- Texture with current frame
- Empty texture if video invalid

**Example:**
```c
RMV_UpdateVideo(video, GetFrameTime());

RM_BeginSurface(surface);
    Texture2D videoTex = RMV_GetVideoTexture(video);
    DrawTexture(videoTex, 0, 0, WHITE);
RM_EndSurface(surface);
```

**Notes:**
- Texture created lazily on first call
- Same texture reused (UpdateTexture internally)
- Do NOT unload the returned texture

---

### RMV_UpdateVideo

```c
void RMV_UpdateVideo(RMV_Video *video, float deltaTime);
```

**Description:**  
Updates video playback. Decodes frames based on delta time.

**Parameters:**
- `video` - Video to update
- `deltaTime` - Time since last update (seconds)

**Example:**
```c
while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    RMV_UpdateVideo(video, dt);
    
    // ... render ...
}
```

**Behavior:**
- Only updates when `state == RMV_STATE_PLAYING`
- Decodes frames to match target FPS
- Handles looping if enabled
- Updates texture automatically

**Frame Timing:**
- Accumulates delta time
- Decodes when accumulated ≥ frame duration
- Prevents frame skipping/dropping

---

### RMV_PlayVideo

```c
void RMV_PlayVideo(RMV_Video *video);
```

**Description:**  
Starts/resumes video playback.

**Parameters:**
- `video` - Video to play

**Example:**
```c
RMV_PlayVideo(video);  // Start playing
```

---

### RMV_PauseVideo

```c
void RMV_PauseVideo(RMV_Video *video);
```

**Description:**  
Pauses video playback.

**Parameters:**
- `video` - Video to pause

---

### RMV_StopVideo

```c
void RMV_StopVideo(RMV_Video *video);
```

**Description:**  
Stops video and resets to beginning.

**Parameters:**
- `video` - Video to stop

**Effects:**
- `state = RMV_STATE_STOPPED`
- `currentTime = 0.0f`

---

### RMV_ToggleVideoPause

```c
void RMV_ToggleVideoPause(RMV_Video *video);
```

**Description:**  
Toggles between playing and paused states.

**Parameters:**
- `video` - Video to toggle

**Example:**
```c
if (IsKeyPressed(KEY_SPACE)) {
    RMV_ToggleVideoPause(video);
}
```

---

### RMV_GetVideoState

```c
RMV_PlaybackState RMV_GetVideoState(const RMV_Video *video);
```

**Description:**  
Gets current playback state.

**Parameters:**
- `video` - Video to query

**Returns:**
- Current state
- `RMV_STATE_ERROR` if video invalid

---

### RMV_IsVideoPlaying

```c
bool RMV_IsVideoPlaying(const RMV_Video *video);
```

**Description:**  
Checks if video is currently playing.

**Parameters:**
- `video` - Video to check

**Returns:**
- `true` if `state == RMV_STATE_PLAYING`
- `false` otherwise

---

### RMV_IsVideoLoaded

```c
bool RMV_IsVideoLoaded(const RMV_Video *video);
```

**Description:**  
Checks if video is loaded and valid.

**Parameters:**
- `video` - Video to check

**Returns:**
- `true` if video is loaded
- `false` if `NULL` or unloaded

---

### RMV_SetVideoLoop

```c
void RMV_SetVideoLoop(RMV_Video *video, bool loop);
```

**Description:**  
Enables/disables video looping.

**Parameters:**
- `video` - Video to configure
- `loop` - `true` to enable looping

**Example:**
```c
RMV_SetVideoLoop(video, true);  // Loop forever
```

**Behavior:**
- When loop enabled: seek to start on EOF
- When loop disabled: stop on EOF

---

## Constants & Macros

### API Prefix

```c
#ifndef RMAPI
    #define RMAPI extern
#endif
```

**Description:**  
API function prefix. Allows DLL export/import customization.

**Custom Usage:**
```c
#define RMAPI __declspec(dllexport)  // Windows DLL
#include "raymap.h"
```

---

### Memory Allocators

```c
#ifndef RMMALLOC
    #define RMMALLOC(sz)        malloc(sz)
#endif
#ifndef RMCALLOC
    #define RMCALLOC(n, sz)     calloc(n, sz)
#endif
#ifndef RMFREE
    #define RMFREE(p)           free(p)
#endif
```

**Description:**  
Memory allocation macros. Override for custom allocators.

**Custom Usage:**
```c
#define RMMALLOC(sz)     my_pool_alloc(pool, sz)
#define RMCALLOC(n,sz)   my_pool_calloc(pool, n, sz)
#define RMFREE(p)        my_pool_free(pool, p)

#define RAYMAP_IMPLEMENTATION
#include "raymap.h"
```

---

### Debug Mode

```c
#ifdef RAYMAP_DEBUG
    RMAPI Mesh *RM_GetSurfaceMesh(RM_Surface *surface);
#endif
```

**Description:**  
Enables debug functions when `RAYMAP_DEBUG` is defined.

**Usage:**
```c
#define RAYMAP_DEBUG
#define RAYMAP_IMPLEMENTATION
#include "raymap.h"

// Access internal mesh for debugging
Mesh *mesh = RM_GetSurfaceMesh(surface);
TraceLog(LOG_INFO, "Vertex count: %d", mesh->vertexCount);
```

** Warning:** Do NOT modify or free the returned mesh!

---

### Internal Constants

```c
#define RM_EPSILON 1e-4f  // Floating-point epsilon
```

**Description:**  
Precision threshold for geometric calculations.

---

## Error Handling

### Logging

RayMap uses Raylib's `TraceLog` for all error reporting.

**Log Levels:**
- `LOG_INFO` - Informational messages
- `LOG_WARNING` - Warnings (operation continues)
- `LOG_ERROR` - Errors (operation fails)
- `LOG_DEBUG` - Debug info (verbose)

**Example:**
```c
SetTraceLogLevel(LOG_ALL);  // Enable all logs

RM_Surface *surf = RM_CreateSurface(10000, 10000, RM_MAP_BILINEAR);
// LOG_ERROR: "RAYMAP: Invalid width 10000 (must be 1-8192)"
```

---

### Return Values

**NULL Pointers:**
- `RM_CreateSurface()` returns `NULL` on failure
- `RMV_LoadVideo()` returns `NULL` on failure

**Boolean Returns:**
- `RM_SetQuad()` returns `false` if quad invalid
- `RM_SaveConfig()` / `RM_LoadConfig()` return `false` on I/O error

**Safe Patterns:**
```c
// Check creation
RM_Surface *surf = RM_CreateSurface(800, 600, RM_MAP_BILINEAR);
if (!surf) {
    TraceLog(LOG_ERROR, "Surface creation failed!");
    return -1;
}

// Check quad validation
if (!RM_SetQuad(surf, newQuad)) {
    TraceLog(LOG_WARNING, "Invalid quad, keeping old one");
}

// Check I/O
if (!RM_LoadConfig(surf, "config.cfg")) {
    TraceLog(LOG_WARNING, "Using default calibration");
}
```

---

### Common Errors

**Invalid Parameters:**
```c
RM_DrawSurface(NULL);  // LOG_WARNING + early return
```

**Dimension Errors:**
```c
RM_CreateSurface(0, 1080, ...);     // LOG_ERROR + NULL
RM_CreateSurface(10000, 1080, ...); // LOG_ERROR + NULL
```

**Quad Validation:**
```c
// Area too small
RM_Quad tiny = {{0,0}, {5,0}, {5,5}, {0,5}};  // 25px²
RM_SetQuad(surf, tiny);  // LOG_WARNING + false

// Corners too close
RM_Quad degenerate = {{100,100}, {100.5,100}, {100.5,100.5}, {100,100.5}};
RM_SetQuad(surf, degenerate);  // LOG_WARNING + false
```

**Resource Exhaustion:**
```c
// GPU out of memory
RM_SetMeshResolution(surf, 256, 256);  // May fail silently if GPU OOM
```

---

## Thread Safety

 **RayMap is NOT thread-safe.**

**Single-threaded only:**
- All functions must be called from the same thread
- Do not create/destroy surfaces from different threads
- Do not update/draw from different threads

**Known Issues:**
- `rmv_GetFFmpegError()` uses static buffer (data race)

**Workarounds:**
- Use mutex if multi-threading needed
- Create separate surfaces per thread (no sharing)

---

## Performance Tips

### Mesh Resolution
```c
// Low detail (fast) - 289 vertices
RM_SetMeshResolution(surface, 16, 16);

// High detail (slow) - 4225 vertices
RM_SetMeshResolution(surface, 64, 64);
```

**Choose based on:**
- Flat surface → 8×8 or 16×16
- Moderate warp → 32×32
- Highly curved → 64×64

### Lazy Updates
```c
// Don't set quad every frame!
if (quadChanged) {
    RM_SetQuad(surface, newQuad);  // Triggers mesh regen
}

// Mesh only regenerates on next RM_DrawSurface()
```

### Video Performance
```c
// Lower resolution = higher FPS
RMV_LoadVideo("720p.mp4");  // Faster than 1080p
```

---

## See Also

- [README.md](../README.md) - Library overview
- [Examples](../examples/) - Code examples
- [Raylib Documentation](https://www.raylib.com/cheatsheet/cheatsheet.html)

---

**Last Updated:** February 2026  
**Version:** 1.1.0
